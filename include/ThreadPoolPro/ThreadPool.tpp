/**
 * @file ThreadPool.tpp
 * @brief ThreadPool template implementation.
 *
 * Contains the implementation of ThreadPool's template member
 * functions: task submission and the atomic-wait-based worker block.
 * Non-template members are implemented in ThreadPool.cpp.
 */

// ============================================================
// Template implementation for ThreadPoolPro::ThreadPool.
// ============================================================
//
//  Sections:
//   1. Task Submission
//   2. Worker Synchronization
//
// ============================================================

// clang-format off
#include <exception> // std::current_exception
#include <stdexcept> // std::runtime_error
#include <tuple>     // std::make_tuple, std::apply
#include <utility>   // std::forward, std::move
// clang-format on

namespace ThreadPoolPro {

// ============================================================
//  Section 1 — Task Submission
// ============================================================

template <typename F, typename... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
    -> Detail::Future<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>> {
    using ReturnType = std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>;
    using State = Detail::ResultState<ReturnType>;

    // Single allocation for the whole round trip — replaces
    // std::packaged_task's own (larger, more general-purpose) internal
    // shared state. Two owners share it: this function's Task closure
    // (released after it runs) and the Future returned below (released
    // on destruction or after get()) — see Detail/Future.h.
    State* state = new State();

    auto runAndPublish = [state, func = std::decay_t<F>(std::forward<F>(f)),
                          argsTuple = std::make_tuple(std::forward<Args>(args)...)]() mutable {
        try {
            if constexpr (std::is_void_v<ReturnType>) {
                std::apply(std::move(func), std::move(argsTuple));
                state->setValue();
            } else {
                state->setValue(std::apply(std::move(func), std::move(argsTuple)));
            }
        } catch (...) {
            state->setException(std::current_exception());
        }

        state->release();
    };

    try {
        submit(Task(std::move(runAndPublish)));
    } catch (...) {
        // submit() (or Task's own construction) threw before the task
        // was ever queued, and we're about to throw out of enqueue()
        // itself — so neither of state's two owners will ever exist to
        // release their share: the closure was destroyed with the Task
        // without running, and the Future below is never constructed.
        // Release both shares here or `state` leaks.
        state->release();
        state->release();
        throw;
    }

    return Detail::Future<ReturnType>(state);
}

template <typename F> void ThreadPool::detach(F&& f) {
    submit(Task(std::decay_t<F>(std::forward<F>(f))));
}

// ============================================================
//  Section 2 — Worker Synchronization
// ============================================================

template <typename Predicate> void ThreadPool::waitUntil(Predicate predicate) noexcept {
    // Spin -> yield -> park on wakeToken_ until predicate() holds. See
    // the doc comment on Detail::spinYieldPark() (Utility.h) for the
    // three-phase rationale and the "never miss a concurrent notify"
    // argument — this is shared verbatim with Detail::ResultState::wait().
    Detail::spinYieldPark(wakeToken_, std::move(predicate));
}

} // namespace ThreadPoolPro
