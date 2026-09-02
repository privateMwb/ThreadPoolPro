// PulseThreadPool VTable operation test suite.
//
// Coverage:
// - invoke_, moveTo_, and destroy_ round-trip correctly for a small,
//   inline-sized callable
// - invoke_ and heapDelete_ correctly destroy and free a large,
//   heap-sized callable
// - moveTo_ and destroy_ also round-trip correctly for the large
//   callable, even though Task itself never calls them on a
//   heap-stored F
//
// These drive VTable's function pointers directly rather than through
// Task, since Task only ever calls moveTo_/destroy_ for inline-stored
// callables and heapDelete_ for heap-stored ones — never both for the
// same F. Testing here at the VTable level, independent of which
// storage strategy Task would pick for a given F, is what lets both
// paths be exercised without contorting Task's own tests.

#include <gtest/gtest.h>

#include <ThreadPoolPro/ThreadPool.h>

#include <cstddef>
#include <new>

using namespace ThreadPoolPro::Detail;

namespace {

// Small enough for inline storage. Tracks invocation and destruction
// counts so moveTo_/destroy_/invoke_ can each be verified directly.
struct VTableSmallCallable {
    int* invokes;
    int* destructions;

    VTableSmallCallable(int* i, int* d) noexcept : invokes{i}, destructions{d} {}

    VTableSmallCallable(VTableSmallCallable&& other) noexcept
        : invokes{other.invokes}, destructions{other.destructions} {
        other.invokes = nullptr;
        // destructions intentionally left wired: the moved-from husk
        // still gets destructed and must report that.
    }

    VTableSmallCallable(const VTableSmallCallable&) = delete;

    ~VTableSmallCallable() {
        if (destructions)
            ++(*destructions);
    }

    void operator()() const {
        if (invokes)
            ++(*invokes);
    }
};

// Padded well past Task's inline capacity. Move-constructible because
// getVTable<F>() compiles all four operations for every F regardless
// of which ones a given test uses.
struct VTableLargeCallable {
    int* invokes;
    int* destructions;
    std::byte padding[64]{};

    VTableLargeCallable(int* i, int* d) noexcept : invokes{i}, destructions{d} {}

    VTableLargeCallable(VTableLargeCallable&& other) noexcept
        : invokes{other.invokes}, destructions{other.destructions} {
        other.invokes = nullptr;
        // destructions intentionally left wired: the moved-from husk
        // still gets destructed and must report that.
    }

    VTableLargeCallable(const VTableLargeCallable&) = delete;

    ~VTableLargeCallable() {
        if (destructions)
            ++(*destructions);
    }

    void operator()() const {
        if (invokes)
            ++(*invokes);
    }
};

} // namespace

// Verifies invoke_, moveTo_, and destroy_ round-trip correctly for a
// small, inline-sized callable, mirroring the exact sequence Task's
// move constructor uses: moveTo_ into the new slot, then destroy_ the
// old one.
TEST(VTableOpsTest, SmallTypeOperationsRoundTrip) {
    int invokes = 0;
    int destructions = 0;

    alignas(std::max_align_t) std::byte src[sizeof(VTableSmallCallable)];
    alignas(std::max_align_t) std::byte dst[sizeof(VTableSmallCallable)];

    ::new (static_cast<void*>(src)) VTableSmallCallable(&invokes, &destructions);

    const VTable* vtable = getVTable<VTableSmallCallable>();

    vtable->invoke_(src);
    EXPECT_EQ(invokes, 1);

    vtable->moveTo_(src, dst);
    vtable->destroy_(src);
    EXPECT_EQ(destructions, 1); // only the moved-from husk destructed so far

    vtable->invoke_(dst);
    EXPECT_EQ(invokes, 2);

    vtable->destroy_(dst);
    EXPECT_EQ(destructions, 2);
}

// Verifies invoke_ and heapDelete_ correctly invoke, destroy, and free
// a large, heap-sized callable.
TEST(VTableOpsTest, LargeTypeHeapDeleteFreesCallable) {
    int invokes = 0;
    int destructions = 0;

    void* p = ::operator new(sizeof(VTableLargeCallable));
    ::new (p) VTableLargeCallable(&invokes, &destructions);

    const VTable* vtable = getVTable<VTableLargeCallable>();

    vtable->invoke_(p);
    EXPECT_EQ(invokes, 1);

    vtable->heapDelete_(p);
    EXPECT_EQ(destructions, 1);
}

// Verifies moveTo_ and destroy_ also round-trip correctly for the
// large callable — Task itself never calls these for a heap-stored F,
// but VTable's operations are independent of storage strategy, and
// this closes the gap left by the type only ever being invoked/
// heap-deleted elsewhere.
TEST(VTableOpsTest, LargeTypeMoveAndDestroy) {
    int invokes = 0;
    int destructions = 0;

    alignas(std::max_align_t) std::byte src[sizeof(VTableLargeCallable)];
    alignas(std::max_align_t) std::byte dst[sizeof(VTableLargeCallable)];

    ::new (static_cast<void*>(src)) VTableLargeCallable(&invokes, &destructions);

    const VTable* vtable = getVTable<VTableLargeCallable>();

    vtable->moveTo_(src, dst);
    vtable->destroy_(src);
    EXPECT_EQ(destructions, 1); // only the moved-from husk destructed so far

    vtable->invoke_(dst);
    EXPECT_EQ(invokes, 1);

    vtable->destroy_(dst);
    EXPECT_EQ(destructions, 2);
}
