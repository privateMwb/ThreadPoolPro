/**
 * @file            Future.h
 *
 * @date            2026-07-25
 *
 * @version         2.0.0
 *
 * @copyright       Copyright (c) 2026 privateMwb
 *                  All rights reserved.
 *                  https://github.com/privateMwb/ThreadPoolPro
 *
 * @attention       This source is released under the MIT license
 *                  SPDX-License-Identifier: MIT
 *                  <http://opensource.org/licenses/MIT>
 */

#pragma once

// clang-format off
#include "Utility.h" // spinYieldPark — the completion flag's spin -> yield -> park wait

#include <atomic>      // std::atomic — the completion flag and refcount
#include <cstdint>     // std::uint32_t — the completion flag's type
#include <exception>   // std::exception_ptr, std::rethrow_exception, std::current_exception
#include <optional>    // std::optional — value storage for non-void T
#include <stdexcept>   // std::logic_error — thrown by get() on an empty Future
#include <utility>     // std::forward, std::move
// clang-format on

// Lightweight move-only future, purpose-built for ThreadPool::enqueue().
// Replaces std::packaged_task + std::future in enqueue(): a single
// manually-refcounted ResultState<T> (one heap allocation total, sized
// for exactly what's needed — a value slot, an exception_ptr, and a
// completion flag) instead of std::packaged_task's own general-purpose
// shared state, and a spin -> yield -> park atomic-wait for get()
// instead of std::future's condition_variable + mutex — the same
// pattern ThreadPool::waitUntil() already uses for worker wakeup.

namespace ThreadPoolPro::Detail {

/**
 * @brief Non-template machinery shared by every `ResultState<T>`
 * specialization: the completion flag, refcount, exception slot, and
 * the spin -> yield -> park wait — everything except the value slot and
 * its type-dependent accessors.
 * @details Inherited privately (never held or deleted through a
 * `ResultStateBase*`) purely to avoid copy-pasting this logic across
 * `ResultState<T>` and `ResultState<void>`; it isn't a polymorphic base
 * and has no virtual destructor; a derived class's `release()` must
 * `delete this` through the derived pointer, not this one.
 */
class ResultStateBase {
  protected:
    ResultStateBase() noexcept : refCount_{2} {}

    ResultStateBase(const ResultStateBase&) = delete;
    ResultStateBase& operator=(const ResultStateBase&) = delete;
    ~ResultStateBase() = default;

    /// @brief Stores an exception and wakes anyone blocked in wait().
    /// Called at most once, by the task's closure, if the task threw
    /// instead of returning normally.
    void setExceptionImpl(std::exception_ptr eptr) noexcept {
        exception_ = std::move(eptr);
        publish();
    }

    /// @brief Marks the state ready and wakes anyone blocked in wait().
    /// Called at most once, by whichever of setValue()/setException()
    /// the task's closure ends up invoking.
    void publish() noexcept {
        ready_.store(1, std::memory_order_release);
        ready_.notify_all();
    }

    /// @brief Blocks until publish() has been called.
    /// @details Spin -> yield -> park, shared with
    /// `ThreadPool::waitUntil()` — see `Detail::spinYieldPark()` in
    /// Utility.h for the rationale. `ready_` doubles here as both the
    /// predicate's condition and the token parked on.
    void wait() noexcept {
        spinYieldPark(ready_, [this] { return ready_.load(std::memory_order_acquire) != 0; });
    }

    /// @brief Releases this owner's share.
    /// @return `true` if this was the last of the two owners — the
    /// caller (the derived class's own `release()`) must then `delete
    /// this` through its own (derived) pointer type.
    [[nodiscard]] bool releaseImpl() noexcept {
        return refCount_.fetch_sub(1, std::memory_order_acq_rel) == 1;
    }

    std::atomic<std::uint32_t> ready_{0};
    std::atomic<int> refCount_;
    std::exception_ptr exception_;
};

/**
 * @brief Shared state between the task producing a result and the
 * Future consuming it.
 * @details Manually reference counted — exactly two owners (the
 * task's closure, and the Future) — rather than a shared_ptr, since
 * that fixed ownership shape doesn't need shared_ptr's general-purpose
 * atomic control block. The last owner to call release() deletes it.
 */
template <typename T> class ResultState : private ResultStateBase {
  public:
    ResultState() noexcept = default;

    ResultState(const ResultState&) = delete;
    ResultState& operator=(const ResultState&) = delete;

    /// @brief Stores the result and wakes anyone blocked in get().
    /// Called at most once, by the task's closure.
    template <typename... Args> void setValue(Args&&... args) {
        value_.emplace(std::forward<Args>(args)...);
        publish();
    }

    /// @brief Stores an exception (in place of a value) and wakes
    /// anyone blocked in get(). Called at most once, by the task's
    /// closure, if the task threw instead of returning normally.
    void setException(std::exception_ptr eptr) noexcept {
        setExceptionImpl(std::move(eptr));
    }

    /// @brief Blocks until a value or exception has been published,
    /// then returns the value (moved out) or rethrows the exception.
    /// Must not be called more than once on the same ResultState.
    [[nodiscard]] T get() {
        wait();

        if (exception_)
            std::rethrow_exception(exception_);

        return std::move(*value_);
    }

    /// @brief Releases this owner's share. The second (last) caller
    /// deletes the state.
    void release() noexcept {
        if (releaseImpl())
            delete this;
    }

  private:
    std::optional<T> value_;
};

/// @brief ResultState<void> — no value slot, just completion + exception.
template <> class ResultState<void> : private ResultStateBase {
  public:
    ResultState() noexcept = default;

    ResultState(const ResultState&) = delete;
    ResultState& operator=(const ResultState&) = delete;

    void setValue() noexcept {
        publish();
    }

    void setException(std::exception_ptr eptr) noexcept {
        setExceptionImpl(std::move(eptr));
    }

    void get() {
        wait();

        if (exception_)
            std::rethrow_exception(exception_);
    }

    void release() noexcept {
        if (releaseImpl())
            delete this;
    }
};

/**
 * @brief Move-only handle to a task's eventual result. Returned by
 * ThreadPool::enqueue() in place of std::future<T> — see the file
 * comment for why.
 */
template <typename T> class Future {
  public:
    Future() noexcept = default;

    /// @brief Takes ownership of one share of `state`. Used internally
    /// by enqueue(); not intended to be constructed directly.
    explicit Future(ResultState<T>* state) noexcept : state_{state} {}

    Future(Future&& other) noexcept : state_{other.state_} {
        other.state_ = nullptr;
    }

    Future& operator=(Future&& other) noexcept {
        if (this != &other) {
            releaseOwnedState();
            state_ = other.state_;
            other.state_ = nullptr;
        }

        return *this;
    }

    Future(const Future&) = delete;
    Future& operator=(const Future&) = delete;

    ~Future() {
        releaseOwnedState();
    }

    /**
     * @brief Blocks until the result is ready, then returns it (or
     * rethrows the task's exception).
     * @throws std::logic_error if called on an empty Future (default-
     * constructed, moved-from, or already get()'d).
     * @details Consumes the Future — must be called at most once, same
     * contract as std::future::get().
     */
    T get() {
        if (!state_)
            throw std::logic_error("Future::get() called on an empty Future");

        ResultState<T>* state = state_;
        state_ = nullptr;

        struct Releaser {
            ResultState<T>* state;
            ~Releaser() {
                state->release();
            }
        } releaser{state};

        return state->get();
    }

    /// @brief Returns whether this Future currently owns a shared state.
    [[nodiscard]] bool valid() const noexcept {
        return state_ != nullptr;
    }

  private:
    void releaseOwnedState() noexcept {
        if (state_) {
            state_->release();
            state_ = nullptr;
        }
    }

    ResultState<T>* state_ = nullptr;
};

} // namespace ThreadPoolPro::Detail
