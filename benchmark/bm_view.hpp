#pragma once

#include "bm_config.hpp"

template <typename tensor_type>
inline void bm_tensor_view_slice(benchmark::State& state) {
    tensor_type ts_src(pointi<tensor_type::rank>::all(state.range(0)));
    auto center = ts_src.shape() / 4;
    auto dst_shape = ts_src.shape() / 2;
    tensor_type ts_dst(dst_shape);

    while (state.KeepRunning()) {
        copy(view::slice(ts_src, center, dst_shape), ts_dst);
        benchmark::DoNotOptimize(ts_dst.data());
    }

    state.SetBytesProcessed(state.iterations() * ts_dst.size() * sizeof(ts_dst[0]));
    state.SetItemsProcessed(state.iterations() * ts_dst.size());
}

template <typename tensor_type>
inline void bm_tensor_view_stride(benchmark::State& state) {
    tensor_type ts_src(pointi<tensor_type::rank>::all(state.range(0)));
    auto center = ts_src.shape() / 4;
    auto dst_shape = ts_src.shape() / 2;
    tensor_type ts_dst(dst_shape);

    while (state.KeepRunning()) {
        copy(view::stride(ts_src, 2), ts_dst);
        benchmark::DoNotOptimize(ts_dst.data());
    }

    state.SetBytesProcessed(state.iterations() * ts_dst.size() * sizeof(ts_dst[0]));
    state.SetItemsProcessed(state.iterations() * ts_dst.size());
}

template <typename tensor_type>
inline void bm_tensor_view_eye(benchmark::State& state) {
    tensor_type ts_dst(pointi<tensor_type::rank>::all(state.range(0)));
    auto center = ts_dst.shape() / 4;

    while (state.KeepRunning()) {
        copy(view::eye<typename tensor_type::value_type>(ts_dst.shape(), ts_dst.runtime(),
                                                         ts_dst.layout()),
             ts_dst);
        benchmark::DoNotOptimize(ts_dst.data());
    }

    state.SetBytesProcessed(state.iterations() * ts_dst.size() * sizeof(ts_dst[0]));
    state.SetItemsProcessed(state.iterations() * ts_dst.size());
}
