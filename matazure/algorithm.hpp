﻿#pragma once

#include <matazure/point.hpp>
#include <matazure/execution.hpp>

namespace matazure {

template <typename _Func>
inline MATAZURE_GENERAL void for_index(sequence_policy policy, int_t first, int_t last, _Func fun) {
	for (int_t i = first; i < last; ++i) {
		fun(i);
	}
}

template <typename _Func>
inline MATAZURE_GENERAL void for_index(sequence_vectorized_policy policy, int_t first, int_t last, _Func fun) {
	#pragma simd //#pragma ivdep
	for (int_t i = first; i < last; ++i) {
		fun(i);
	}
}

#ifdef _OPENMP

template <typename _Func>
inline MATAZURE_GENERAL void for_index(omp_policy policy, int_t first, int_t last, _Func fun){
	#pragma omp parallel for
	for (int_t i = first; i < last; ++i) {
		fun(i);
	}
}

template <typename _Func>
inline MATAZURE_GENERAL void for_index(omp_vectorized_policy policy, int_t first, int_t last, _Func fun){
	#pragma simd
	#pragma omp parallel for
	for (int_t i = first; i < last; ++i) {
		fun(i);
	}
}

#endif

template <typename _Func>
inline MATAZURE_GENERAL void for_index(int_t first, int_t last, _Func fun) {
	sequence_policy seq{};
	for_index(seq, first, last, fun);
}

template <typename _Func>
inline MATAZURE_GENERAL void for_index(pointi<1> origin, pointi<1> extent, _Func fun) {
	for (int_t i = origin[0]; i < extent[0]; ++i) {
		fun(pointi<1>{ { i } });
	}
}

template <typename _Func>
inline MATAZURE_GENERAL void for_index(pointi<2> origin, pointi<2> extent, _Func fun) {
	for (int_t j = origin[1]; j < extent[1]; ++j) {
		for (int_t i = origin[0]; i < extent[0]; ++i) {
			fun(pointi<2>{ { i, j } });
		}
	}
}

template <typename _Func>
inline MATAZURE_GENERAL void for_index(pointi<3> origin, pointi<3> extent, _Func fun) {
	for (int_t k = origin[2]; k < extent[2]; ++k) {
		for (int_t j = origin[1]; j < extent[1]; ++j) {
			for (int_t i = origin[0]; i < extent[0]; ++i) {
				fun(pointi<3>{ { i, j, k } });
			}
		}
	}
}

template <typename _Func>
inline MATAZURE_GENERAL void for_index(pointi<4> origin, pointi<4> extent, _Func fun) {
	for (int_t l = origin[3]; l < extent[3]; ++l) {
		for (int_t k = origin[2]; k < extent[2]; ++k) {
			for (int_t j = origin[1]; j < extent[1]; ++j) {
				for (int_t i = origin[0]; i < extent[0]; ++i) {
					fun(pointi<4>{ {i, j, k, l} });
				}
			}
		}
	}
}

template <typename _Tensor, typename _Fun>
inline MATAZURE_GENERAL void for_each(_Tensor &ts, _Fun fun, enable_if_t<are_linear_access<_Tensor>::value && none_device_memory<_Tensor>::value>* = 0) {
	for_index(0, ts.size(), [&](int_t i) {
		fun(ts[i]);
	});
}

template <typename _Tensor, typename _Fun>
inline MATAZURE_GENERAL void for_each(_Tensor &ts, _Fun fun, enable_if_t<!are_linear_access<_Tensor>::value && none_device_memory<_Tensor>::value>* = 0) {
	for_index(pointi<_Tensor::rank>::zeor(), ts.shape(), [&](pointi<_Tensor::rank> idx) {
		fun(ts(idx));
	});
}

template <typename _Tensor>
inline MATAZURE_GENERAL void fill(_Tensor &ts, typename _Tensor::value_type v, enable_if_t<none_device_memory<_Tensor>::value>* = 0) {
	for_each(ts, [v](typename _Tensor::value_type &x) { x = v;});
}

template <typename _T1, typename _T2>
inline MATAZURE_GENERAL void copy(const _T1 &lhs, _T2 &rhs, enable_if_t<are_linear_access<_T1, _T2>::value && none_device_memory<_T1, _T2>::value>* = 0) {
	for_index(0, lhs.size(), [&](int_t i) {
		rhs[i] = lhs[i];
	});
}

template <typename _T1, typename _T2>
inline MATAZURE_GENERAL void copy(const _T1 &lhs, _T2 &rhs, enable_if_t<!are_linear_access<_T1, _T2>::value && none_device_memory<_T1, _T2>::value>* = 0) {
	for_index(pointi<_T1::rank>::zeros(), lhs.shape(), [&](pointi<_T1::rank> idx) {
		rhs(idx) = lhs(idx);
	});
}

template <typename _T1, typename _T2, typename _TransFun>
inline MATAZURE_GENERAL void transform(const _T1 &lhs, _T2 &rhs, _TransFun fun, enable_if_t<are_linear_access<_T1, _T2>::value && none_device_memory<_T1, _T2>::value>* = 0) {
	for_index(0, lhs.size(), [&](int_t i) {
		fun(lhs[i], rhs[i]);
	});
}

template <typename _T1, typename _T2, typename _TransFun>
inline MATAZURE_GENERAL void transform(const _T1 &lhs, _T2 &rhs, _TransFun fun, enable_if_t<!are_linear_access<_T1, _T2>::value && none_device_memory<_T1, _T2>::value>* = 0) {
	for_index(pointi<_T1::rank>::zeros(), lhs.shape(), [&](pointi<_T1::rank> idx) {
		fun(lhs(idx), rhs(idx));
	});
}

template <typename _T1, typename _T2>
inline MATAZURE_GENERAL bool equal(_T1 lhs, _T2 rhs, enable_if_t<none_device_memory<_T1, _T2>::value>* = 0) {
	for (int_t i = 0; i < lhs.size(); ++i) {
		if (MATAZURE_UNLIKELY(lhs[i] != rhs[i])) return false;
	}

	return true;
}
//
// template <typename _T1, typename _T2, typename _TransFun>
// inline MATAZURE_GENERAL void equal(_T1 lhs, _T2 rhs, _TransFun fun, enable_if_t<!are_linear_access<_T1, _T2>::value && none_device_memory<_T1, _T2>::value>* = 0) {
	//not implement
// }

template <typename _Tensor, typename _VT, typename _BinaryOp>
inline MATAZURE_GENERAL _VT reduce(const _Tensor &ts, _VT init, _BinaryOp binaryop, enable_if_t<none_device_memory<_Tensor>::value>* = 0) {
	auto re = init;
	for_each(ts, [&re, binaryop](decltype(ts[0]) x) {
		re = binaryop(re, x);
	});

	return re;
}

template <typename _TS>
inline MATAZURE_GENERAL auto sum(const _TS &ts)->typename _TS::value_type {
	typedef typename _TS::value_type value_type;
	return reduce(ts, zero<value_type>::value(), [&](value_type lhs, value_type rhs) {
		return lhs + rhs;
	});
}

template <typename _TS>
inline MATAZURE_GENERAL typename _TS::value_type prod(const _TS &ts) {
	typedef typename _TS::value_type value_type;
	return reduce(ts, value_type(1), [&](value_type lhs, value_type rhs) {
		return lhs * rhs;
	});
}

template <typename _TS>
inline MATAZURE_GENERAL auto mean(const _TS &ts)->typename _TS::value_type {
	return sum(ts) / ts.size();
}

template <typename _TS>
inline MATAZURE_GENERAL typename _TS::value_type max(const _TS &ts) {
	typedef typename _TS::value_type value_type;
	return reduce(ts, ts[0], [&](value_type lhs, value_type rhs) {
		return rhs > lhs ? rhs : lhs;
	});
}

template <typename _TS>
inline MATAZURE_GENERAL typename _TS::value_type min(const _TS &ts) {
	typedef typename _TS::value_type value_type;
	return reduce(ts, numeric_limits<value_type>::max(), [&](value_type lhs, value_type rhs) {
		return lhs <= rhs ? lhs : rhs;
	});
}

}
