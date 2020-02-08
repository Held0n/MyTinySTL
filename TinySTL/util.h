//
// 包含一些通用的工具
// 包括 move, forward, swap 等函数，以及 pair
//

#ifndef TINYSTL_UTIL_H
#define TINYSTL_UTIL_H

#include <cstddef>
#include "type_traits.h"

namespace mystl {
    // move 函数
    template <class T>
    typename std::remove_reference<T>::type&& move(T&& arg) noexcept {
        return static_cast<typename std::remove_reference<T>::type&&>(arg);
    }

    // forward 函数
    template <class T>
    T&& forward(typename std::remove_reference<T>::type& arg) noexcept {
        return static_cast<T&&>(arg);
    }

    template <class T>
    T&& forward(typename std::remove_reference<T>::type&& arg) noexcept {
        static_assert(!std::is_lvalue_reference<T>::value, "bad forward");
        return static_cast<T&&>(arg);
    }

    // swap 函数
    template <class Tp>
    void swap(Tp& lhs, Tp& rhs) {
        auto tmp(mystl::move(lhs));
        lhs = mystl::move(rhs);
        rhs = mystl::move(lhs);
    }

    template <class ForwardIter1, class ForwardIter2>
    ForwardIter2 swap_range(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2) {
        for (; first1 != last1; ++first1, (void) ++first2) {
            mystl::swap(*first1, *first2);
        }
        return first2;
    }

    template <class Tp, size_t N>
    void swap(Tp(&a)[N], Tp(&b)[N]) {
        mystl::swap_range(a, a+N, b);
    }

    // pair
    // 模板结构体 pair
    // 两个模板分别为两个元素的类型
    // 用 first 和 second 分别取出第一和第二个元素
    template <class Ty1, class Ty2>
    struct pair {
        typedef Ty1 first_type;
        typedef Ty2 second_type;

        first_type first;   // first 是第一个元素
        second_type second; // second 是第二个元素

        // 默认的构造函数
        // std::is_default_constructible: 检查类型是否有默认的构造函数
        template <class Other1 = Ty1, class Other2 = Ty2,
            typename = typename std::enable_if<
            std::is_default_constructible<Other1>::value &&
            std::is_default_constructible<Other2>::value, void>::type>
        constexpr pair() : first(), second() {}

        // 隐式的构造函数
        // std::is_copy_constructible: 检查类型是否有默认的拷贝构造函数
        // std::is_convertible: 检查一个类型是否能转换为另一个类型
        template <class U1 = Ty1, class U2 = Ty2,
            typename std::enable_if<
            std::is_copy_constructible<U1>::value &&
            std::is_copy_constructible<U2>::value &&
            std::is_convertible<const U1&, Ty1>::value &&
            std::is_convertible<const U2&, Ty2>::value, int>::type = 0>
        constexpr pair(const Ty1& a, const Ty2& b) : first(a), second(b) {}

        // 显式的构造函数
        template <class U1 = Ty1, class U2 = Ty2,
            typename std::enable_if<
            std::is_copy_constructible<U1>::value &&
            std::is_copy_constructible<U2>::value &&
            (!std::is_convertible<const U1&, Ty1>::value ||
             !std::is_convertible<const U2&, Ty2>::value), int>::type = 0>
        explicit constexpr pair(const Ty1& a, const Ty2& b) : first(a), second(b) {}

        /* 对于其他类型 */
        pair(const pair& rhs) = default;
        pair(pair&& rhs) = default;

        // 对于其他类型的隐式构造函数
        template <class Other1, class Other2,
            typename std::enable_if<
            std::is_constructible<Ty1, Other1>::value &&
            std::is_constructible<Ty2, Other2>::value &&
            std::is_convertible<Other1&&, Ty1>::value &&
            std::is_convertible<Other2&&, Ty2>::value, int>::type = 0>
        constexpr pair(Other1&& a, Other2&& b) : first(mystl::forward<Other1>(a)),
                                                 second(mystl::forward<Other2>(b)) {}

        // 对于其他类型的显式构造函数
        template <class Other1, class Other2,
            typename std::enable_if<
            std::is_constructible<Ty1, Other1>::value &&
            std::is_constructible<Ty2, Other2>::value &&
            (!std::is_convertible<Other1&&, Ty1>::value ||
             !std::is_convertible<Other2&&, Ty2>::value), int>::type = 0>
        explicit constexpr pair(Other1&& a, Other2&& b) : first(mystl::forward<Other1>(a)),
                                                          second(mystl::forward<Other2>(b)) {}

        // 对于其他 pair 的隐式构造函数
        template <class Other1, class Other2,
            typename std::enable_if<
            std::is_constructible<Ty1, const Other1&>::value &&
            std::is_constructible<Ty2, const Other2&>::value &&
            std::is_convertible<const Other1&, Ty1>::value &&
            std::is_convertible<const Other2&, Ty2>::vlaue, int>::type = 0>
        constexpr pair(const pair<Other1, Other2>& other) : first(other.first),
                                                      second(other.second) {}

        // 对于其他 pair 的显式构造函数
        template <class Other1, class Other2,
            typename std::enable_if<
            std::is_constructible<Ty1, Other1>::value &&
            std::is_constructible<Ty2, Other2>::value &&
            (!std::is_convertible<Other1, Ty1>::value ||
             !std::is_convertible<Other2, Ty2>::vlaue), int>::type = 0>
        explicit constexpr pair(const pair<Other1, Other2>& other) : first(other.first),
                                                                     second(other.second) {}

        // 对于其他 pair 的隐式构造函数
        template <class Other1, class Other2,
            typename std::enable_if<
            std::is_constructible<Ty1, const Other1&>::value &&
            std::is_constructible<Ty2, Other2>::value &&
            std::is_convertible<Other1, Ty1>::value &&
            std::is_convertible<Other2, Ty2>::vlaue, int>::type = 0>
        constexpr pair(pair<Other1, Other2>&& other) : first(mystl::forward<Other1>(other.first)),
                                                             second(mystl::forward<Other2>(other.second)) {}

        // 对于其他 pair 的显式构造函数
        template <class Other1, class Other2,
            typename std::enable_if<
            std::is_constructible<Ty1, Other1>::value &&
            std::is_constructible<Ty2, Other2>::value &&
            (!std::is_convertible<Other1, Ty1>::value ||
             !std::is_convertible<Other2, Ty2>::vlaue), int>::type = 0>
        explicit constexpr pair(pair<Other1, Other2>&& other) : first(mystl::forward<Other1>(other.first)),
                                                                      second(mystl::forward<Other2>(other.second)) {}
        // 重载 = 操作符， 拷贝赋值
        template <class Other1, class Other2>
        pair& operator=(const pair<Other1, Other2>& other) {
            first = other.first;
            second = other.second;
            return *this;
        }

        // 重载 = 操作符，move
        template <class Other1, class Other2>
        pair& operator=(pair<Other1, Other2>&& other) {
            first = mystl::forward<Other1>(other.first);
            second = mystl::forward<Other2>(other.second);
            return *this;
        }

        // 析构函数
        ~pair() = default;

        // swap 函数
        void swap(pair& other) {
            if(this != &other) {
                mystl::swap(first, other.first);
                mystl::swap(second, other.second);
            }
        }
    }; // struct pair

    // 重载 == 操作符
    template <class Ty1, class Ty2>
    bool operator==(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs) {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }

    // 重载 < 操作符
    template <class Ty1, class Ty2>
    bool operator<(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs) {
        return lhs.first < rhs.first || (lhs.first == rhs.first && lhs.second < rhs.second);
    }

    // 重载 != 操作符
    template <class Ty1, class Ty2>
    bool operator!=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs) {
        return !(lhs == rhs);
    }


    // 重载 > 操作符
    template <class Ty1, class Ty2>
    bool operator>(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs) {
        return rhs < lhs;
    }


    // 重载 <= 操作符
    template <class Ty1, class Ty2>
    bool operator<=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs) {
        return !(lhs > rhs);
    }

    // 重载 >= 操作符
    template <class Ty1, class Ty2>
    bool operator>=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs) {
        return !(lhs < rhs);
    }

    // 重载 mystl 的 swap
    template <class Ty1, class Ty2>
    void swap(pair<Ty1, Ty2>& lhs, pair<Ty1, Ty2>& rhs) {
        lhs.swap(rhs);
    }

    // make_pair函数
    template <class Ty1, class Ty2>
    pair<Ty1, Ty2> make_pair(Ty1&& first, Ty2&& second) {
        return pair<Ty1, Ty2>(mystl::forward<Ty1>(first), mystl::forward<Ty2>(second));
    }
} // namespace mystl

#endif //TINYSTL_UTIL_H
