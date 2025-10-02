//
// Created by Administrator on 2025/10/2.
//

#ifndef PARAMGENERATOR_SOFT_DIFF_H
#define PARAMGENERATOR_SOFT_DIFF_H

#pragma once
#include <cmath>
#include <type_traits>

// 非周期性“软差值” (arctan 方案)
// f(x) = alpha * s * atan((x - center) / s)
// f'(x) = alpha / (1 + ((x - center)/s)^2)
//
// 参数说明：
// - center: 中间值 c
// - s     : 距离尺度，越大衰减越慢；要满足 |x - c|≈300 时斜率≈0.1，建议 s=100 且 alpha=1
// - alpha : 中心斜率（f'(c)），默认 1
template <class T>
constexpr T softDiffAtan(T x, T center, T s, T alpha = T(1)) {
    static_assert(std::is_floating_point<T>::value, "T must be floating-point");
    const T d = x - center;
    return alpha * s * std::atan(d / s);
}

// 计算增量率（导数），用于验证或自适应调参
template <class T>
constexpr T softDiffAtanSlope(T x, T center, T s, T alpha = T(1)) {
    const T u = (x - center) / s;
    return alpha / (T(1) + u * u);
}

// 根据目标斜率 targetSlope 和目标距离 targetDelta 求 s（alpha 已知）
// 公式：targetSlope = alpha / (1 + (targetDelta/s)^2) => s = targetDelta / sqrt(alpha/targetSlope - 1)
template <class T>
constexpr T solveSForSlope(T targetDelta, T targetSlope, T alpha = T(1)) {
    return targetDelta / std::sqrt(alpha / targetSlope - T(1));
}

// 备选：非周期“余弦感”更强的方案 (tanh 方案)
// f(x) = alpha * s * tanh((x - center)/s)
// f'(x) = alpha * sech^2((x - center)/s)
template <class T>
constexpr T softDiffTanh(T x, T center, T s, T alpha = T(1)) {
    const T u = (x - center) / s;
    // std::tanh 可用；sech^2 仅用于说明导数形状，函数本体用 tanh 即可
    return alpha * s * std::tanh(u);
}

template <class T>
constexpr T softDiffTanhSlope(T x, T center, T s, T alpha = T(1)) {
    const T u = (x - center) / s;
    const T t = std::tanh(u);
    // sech^2(u) = 1 - tanh^2(u)
    return alpha * (T(1) - t * t);
}


#endif //PARAMGENERATOR_SOFT_DIFF_H