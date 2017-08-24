/*
    tests/memory.cpp -- tests for load/store/gather/scatter, etc.

    Enoki is a C++ template library that enables transparent vectorization
    of numerical kernels using SIMD instruction sets available on current
    processor architectures.

    Copyright (c) 2017 Wenzel Jakob <wenzel.jakob@epfl.ch>

    All rights reserved. Use of this source code is governed by a BSD-style
    license that can be found in the LICENSE file.
*/

#include "test.h"

ENOKI_TEST_ALL(test01_load) {
    alignas(alignof(T)) Value mem[Size];
    for (size_t i = 0; i < Size; ++i)
        mem[i] = (Value) i;
    assert(load<T>(mem) == index_sequence<T>());
}

ENOKI_TEST_ALL(test02_load_unaligned) {
    Value mem[Size];
    for (size_t i = 0; i < Size; ++i)
        mem[i] = (Value) i;
    assert(load_unaligned<T>(mem) == index_sequence<T>());
}

ENOKI_TEST_ALL(test03_store) {
    alignas(alignof(T)) Value mem[Size];
    store(mem, index_sequence<T>());
    for (size_t i = 0; i < Size; ++i)
        assert(mem[i] == (Value) i);
}

ENOKI_TEST_ALL(test04_store_unaligned) {
    Value mem[Size];
    store_unaligned(mem, index_sequence<T>());
    for (size_t i = 0; i < Size; ++i)
        assert(mem[i] == (Value) i);
}

ENOKI_TEST_ALL(test05_gather) {
    alignas(alignof(T)) Value mem[Size], dst[Size];
    uint32_t indices32[Size];
    uint64_t indices64[Size];
    for (size_t i = 0; i < Size; ++i) {
        indices32[i] = uint32_t(Size - 1 - i);
        indices64[i] = uint64_t(Size - 1 - i);
        mem[i] = (Value) i;
    }

    auto id32 = load_unaligned<Array<uint32_t, Size>>(indices32);
    auto id64 = load_unaligned<Array<uint64_t, Size>>(indices64);

#if defined(_MSC_VER)
    /// MSVC doesn't seem to correctly track data dependencies involving gathers
    std::cout << mem[0] << std::endl;
#endif

    store(dst, gather<T>(mem, id32));
    for (size_t i = 0; i < Size; ++i)
        assert(dst[i] == Value(Size - 1 - i));
    memset(dst, 0, sizeof(Value) * Size);


#if defined(_MSC_VER)
    /// MSVC doesn't seem to correctly track data dependencies involving gathers
    std::cout << mem[0] << std::endl;
#endif

    store(dst, gather<T>(mem, id64));

    for (size_t i = 0; i < Size; ++i)
        assert(dst[i] == Value(Size - 1 - i));
    memset(dst, 0, sizeof(Value) * Size);
}

ENOKI_TEST_ALL(test06_gather_mask) {
    alignas(alignof(T)) Value mem[Size], dst[Size];
    uint32_t indices32[Size];
    uint64_t indices64[Size];
    for (size_t i = 0; i < Size; ++i) {
        indices32[i] = uint32_t(Size - 1 - i);
        indices64[i] = uint64_t(Size - 1 - i);
        mem[i] = (Value) i;
    }

    auto id32 = load_unaligned<Array<uint32_t, Size>>(indices32);
    auto id64 = load_unaligned<Array<uint64_t, Size>>(indices64);
    auto idx = index_sequence<uint_array_t<T>>();
    auto even_mask = mask_t<T>(eq(sli<1>(sri<1>(idx)), idx));

#if defined(_MSC_VER)
    /// MSVC doesn't seem to correctly track data dependencies involving gathers
    std::cout << mem[0] << std::endl;
#endif

    memset(dst, 0, sizeof(Value) * Size);
    store(dst, gather<T>(mem, id32, even_mask));
    for (size_t i = 0; i < Size; ++i)
        assert(dst[i] == ((i % 2 == 0) ? Value(Size - 1 - i) : 0));
    memset(dst, 0, sizeof(Value) * Size);

#if defined(_MSC_VER)
    /// MSVC doesn't seem to correctly track data dependencies involving gathers
    std::cout << mem[0] << std::endl;
#endif

    store(dst, gather<T>(mem, id64, even_mask));
    for (size_t i = 0; i < Size; ++i)
        assert(dst[i] == ((i % 2 == 0) ? Value(Size - 1 - i) : 0));
}

ENOKI_TEST_ALL(test07_scatter) {
    alignas(alignof(T)) Value mem[Size], dst[Size];
    uint32_t indices32[Size];
    uint64_t indices64[Size];
    for (size_t i = 0; i < Size; ++i) {
        indices32[i] = uint32_t(Size - 1 - i);
        indices64[i] = uint64_t(Size - 1 - i);
        mem[i] = (Value) i;
    }

    auto id32 = load_unaligned<Array<uint32_t, Size>>(indices32);
    auto id64 = load_unaligned<Array<uint64_t, Size>>(indices64);

    memset(dst, 0, sizeof(Value) * Size);
    scatter(dst, load<T>(mem), id32);
    for (size_t i = 0; i < Size; ++i)
        assert(dst[i] == Value(Size - 1 - i));
    memset(dst, 0, sizeof(Value) * Size);

    scatter(dst, load<T>(mem), id64);
    for (size_t i = 0; i < Size; ++i)
        assert(dst[i] == Value(Size - 1 - i));
    memset(dst, 0, sizeof(Value) * Size);
}

ENOKI_TEST_ALL(test08_scatter_mask) {
    alignas(alignof(T)) Value mem[Size], dst[Size];
    uint32_t indices32[Size];
    uint64_t indices64[Size];
    for (size_t i = 0; i < Size; ++i) {
        indices32[i] = uint32_t(Size - 1 - i);
        indices64[i] = uint64_t(Size - 1 - i);
        mem[i] = (Value) i;
    }
    auto id32 = load_unaligned<Array<uint32_t, Size>>(indices32);
    auto id64 = load_unaligned<Array<uint64_t, Size>>(indices64);

    auto idx = index_sequence<uint_array_t<T>>();
    auto even_mask = mask_t<T>(eq(sli<1>(sri<1>(idx)), idx));

    memset(dst, 0, sizeof(Value) * Size);
    scatter(dst, load<T>(mem), id32, even_mask);
    for (size_t i = 0; i < Size; ++i)
        assert(dst[i] == (((Size-1-i) % 2 == 0) ? Value(Size - 1 - i) : 0));
    memset(dst, 0, sizeof(Value) * Size);

    scatter(dst, load<T>(mem), id64, even_mask);
    for (size_t i = 0; i < Size; ++i)
        assert(dst[i] == (((Size-1-i) % 2 == 0) ? Value(Size - 1 - i) : 0));
}

ENOKI_TEST_ALL(test09_prefetch) {
    alignas(alignof(T)) Value mem[Size];
    uint32_t indices32[Size];
    uint64_t indices64[Size];
    for (size_t i = 0; i < Size; ++i) {
        indices32[i] = uint32_t(Size - 1 - i);
        indices64[i] = uint64_t(Size - 1 - i);
        mem[i] = (Value) i;
    }
    auto id32 = load_unaligned<Array<uint32_t, Size>>(indices32);
    auto id64 = load_unaligned<Array<uint64_t, Size>>(indices64);
    auto idx = index_sequence<uint_array_t<T>>();
    auto even_mask = mask_t<T>(eq(sli<1>(sri<1>(idx)), idx));

    /* Hard to test these, let's at least make sure that it compiles
       and does not crash .. */
    prefetch<T>(mem, id32);
    prefetch<T>(mem, id64);
    prefetch<T>(mem, id32, even_mask);
    prefetch<T>(mem, id64, even_mask);
}

ENOKI_TEST_ALL(test10_load_masked) {
    alignas(alignof(T)) Value mem[Size];
    Value mem_u[Size];
    Value mem2[Size];
    for (size_t i = 0; i < Size; ++i) {
        mem[i] = (Value) i;
        mem_u[i] = (Value) i;
        mem2[i] = (i % 2 == 0) ? (Value) i : (Value) 0;
    }
    auto idx = index_sequence<uint_array_t<T>>();
    auto even_mask = mask_t<T>(eq(sli<1>(sri<1>(idx)), idx));
    assert(load<T>(mem, even_mask) == load_unaligned<T>(mem2));
    assert(load_unaligned<T>(mem_u, even_mask) == load_unaligned<T>(mem2));
}

ENOKI_TEST_ALL(test11_store_masked) {
    alignas(alignof(T)) Value mem[Size];
    Value mem_u[Size];
    Value mem2[Size];
    for (size_t i = 0; i < Size; ++i) {
        mem[i] = 1;
        mem_u[i] = 1;
        mem2[i] = (i % 2 == 0) ? (Value) i : (Value) 1;
    }

    auto idx = index_sequence<uint_array_t<T>>();
    auto idx2 = index_sequence<T>();
    auto even_mask = mask_t<T>(eq(sli<1>(sri<1>(idx)), idx));

#if defined(_MSC_VER)
    /// MSVC doesn't seem to correctly track data dependencies involving masked loads/stores
    for (size_t i = 0; i < Size; ++i)
        std::cout << mem[i] << " " << mem_u[i] << std::endl;
#endif

    store(mem, idx2, even_mask);
    store_unaligned(mem_u, idx2, even_mask);

    assert(load_unaligned<T>(mem) == load_unaligned<T>(mem2));
    assert(load_unaligned<T>(mem_u) == load_unaligned<T>(mem2));
}
