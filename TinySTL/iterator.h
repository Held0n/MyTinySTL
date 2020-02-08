//
// 有关迭代器设计的头文件
//
#ifndef TINYSTL_ITERATOR_H
#define TINYSTL_ITERATOR_H

// ptrdiff_t
#include <cstddef>
#include "type_traits.h"

namespace mystl {
    // 五种迭代器类型

    // 输入迭代器
    struct input_iterator_tag{};
    // 输出迭代器
    struct output_iterator_tag{};
    // 前向迭代器
    struct forward_iterator_tag : public input_iterator_tag{};
    // 双向迭代器
    struct bidirectional_iterator_tag : public forward_iterator_tag{};
    // 随机访问迭代器
    struct random_access_iterator_tag : public bidirectional_iterator_tag{};

    // iterator 模板
    template <class Category, class T, class Distance = ptrdiff_t,class Pointer = T*, class Reference = T&>
    struct iterator{
        typedef Category  iterator_category;
        typedef T         value_type;
        typedef Pointer   pointer;
        typedef Reference reference;
        typedef Distance  difference_type;
    };

    // iterator traits 类型萃取
    template <class T>
    struct has_iterator_cat {
    private:
        struct two {
            char a;
            char b;
        };
        template <class U>
        static two test(...);

        template <class U>
        static char test(typename U::iterator_categorly* = 0);

    public:
        static const bool value = sizeof(test<T>(0)) == sizeof(char);
    };

    template <class Iterator, bool>
    struct iterator_traits_impl {};

    // iterator_traits_impl 的偏特化
    template <class Iterator>
    struct iterator_traits_impl<Iterator, true> {
        typedef typename Iterator::iterator_category iterator_category;
        typedef typename Iterator::value_type        value_type;
        typedef typename Iterator::pointer           pointer;
        typedef typename Iterator::reference         reference;
        typedef typename Iterator::difference_type   difference_type;
    };

    template <class Iterator, bool>
    struct iterator_traits_helper {};

    // iterator_traits 的偏特化
    template <class Iterator>
    struct iterator_traits_helper<Iterator, true>
            : public iterator_traits_helper<Iterator,
            std::is_convertible<typename Iterator::iterator_category, input_iterator_tag>::value ||
            std::is_convertible<typename Iterator::iterator_category, output_iterator_tag>::value> {};

    // 萃取迭代器的类型
    template <class Iterator>
    struct iterator_traits
            : public iterator_traits_helper<Iterator, has_iterator_cat<Iterator>::value> {};

    // 对原生指针的偏特化
    template <class T>
    struct iterator_traits<T*> {
        typedef random_access_iterator_tag iterator_category;
        typedef T                          value_type;
        typedef T*                         pointer;
        typedef T&                         reference;
        typedef ptrdiff_t                  difference_type;
    };

    // 对const 指针的偏特化
    template <class T>
    struct iterator_traits<const T*> {
        typedef random_access_iterator_tag iterator_category;
        typedef T                          value_type;
        typedef const T*                   pointer;
        typedef const T&                   reference;
        typedef ptrdiff_t                  difference_type;
    };

    template <class T, class U, bool = has_iterator_cat<iterator_traits<T>>::value>
    struct has_iterator_cat_of
        : public m_bool_constant<std::is_convertible<typename iterator_traits<T>::iterator_category, U>::value> {};

    //萃取迭代器的类型
    template <class T, class U>
    struct has_iterator_cat_of<T, U, false> : public m_false_type {};

    template <class Iter>
    struct is_input_iterator
        : public has_iterator_cat_of<Iter, input_iterator_tag> {};

    template <class Iter>
    struct is_output_iterator
        : public has_iterator_cat_of<Iter, output_iterator_tag> {};

    template <class Iter>
    struct is_forward_iterator
        : public has_iterator_cat_of<Iter, forward_iterator_tag> {};

    template <class Iter>
    struct is_bidirectional_iterator
        : public has_iterator_cat_of<Iter, bidirectional_iterator_tag> {};

    template <class Iter>
    struct is_random_access_iterator
        : public has_iterator_cat_of<Iter, random_access_iterator_tag> {};

    template <class Iterator>
    struct is_iterator
        : public m_bool_constant<is_input_iterator<Iterator>::value ||
            is_output_iterator<Iterator>::value> {};

    // 萃取某个迭代器的 category
    template <class Iterator>
    typename iterator_traits<Iterator>::iterator_category
    iterator_category(const Iterator&) {
        typedef typename iterator_traits<Iterator>::iterator_category Category;
        return Category();
    }

    // 萃取某个迭代器的 difference_type
    template <class Iterator>
    typename iterator_traits<Iterator>::difference_type*
    distance_type(const Iterator&) {
        return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
    }

    // 萃取某个迭代器的 value_type
    template <class Iterator>
    typename iterator_traits<Iterator>::value_type*
    value_type(const Iterator&) {
        return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
    }

    /* 以下函数用于计算 Iterator 的 distance */
    // input_iterator_tag 的 distance
    template <class InputIterator>
    typename iterator_traits<InputIterator>::difference_type
    distance_dispatch(InputIterator first, InputIterator last, input_iterator_tag) {
        typename iterator_traits<InputIterator>::difference_type  n = 0;
        while (first != last) {
            ++first;
            ++n;
        }
        return n;
    }

    // random_access_iterator 的 distance
    template <class RandomIter>
    typename iterator_traits<RandomIter>::difference_type
    distance_dispatch(RandomIter first, RandomIter last, random_access_iterator_tag) {
        return last - first;
    }

    // input_iterator_tag 的 distance 的接口
    template <class InputIterator>
    typename iterator_traits<InputIterator>::difference_type
    distance(InputIterator first, InputIterator last) {
        return distance_dispatch(first, last, iterator_category(first));
    }

    /* 以下函数用于让迭代器增加 n 个距离 */
    // input_iterator_tag 的 advance
    template <class InputIterator, class Distance>
    void advance_dispatch(InputIterator& i, Distance n, input_iterator_tag) {
        while (n--) {
            ++i;
        }
    }

    // bidirectional_iterator_tag 的 advance
    template <class BidirectionalIterator, class Distance>
    void advance_dispatch(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag) {
        if (n >= 0) {
            while (n--) ++i;
        } else {
            while (n++) --i;
        }
    }

    // random_access_iterator_tag 的 advance
    template <class RandomIter, class Distance>
    void advance_dispatch(RandomIter& i, Distance n, random_access_iterator_tag) {
        i += n;
    }

    // advance 接口
    template <class InputIterator, class Distance>
    void advance(InputIterator& i, Distance n) {
        advance_dispatch(i, n, iterator_category(i));
    }

    // 模板类： reverse_iterator
    // 反向迭代器
    template <class Iterator>
    class reverse_iterator {
    private:
        Iterator current; // 记录对应的正向迭代器
    public:
        // 反向迭代器对应的5种类型
        typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
        typedef typename iterator_traits<Iterator>::value_type        value_type;
        typedef typename iterator_traits<Iterator>::difference_type   difference_type;
        typedef typename iterator_traits<Iterator>::pointer           pointer;
        typedef typename iterator_traits<Iterator>::reference         reference;

        typedef Iterator                   iterator_type;
        typedef reverse_iterator<Iterator> self;

    public:
        // 构造函数
        reverse_iterator() {}
        explicit reverse_iterator(iterator_type i) : current(i) {}
        reverse_iterator(const self& rhs) : current(rhs.current) {}

    public:
        // 取出对应的正向迭代器
        iterator_type base() const {
            return current;
        }

        // 重载 * 操作符
        reference operator*() const {
            // 实际对应正向迭代器的前一个位置
            auto tmp = current;
            return *--tmp;
        }

        // 重载 -> 操作符
        pointer operator->() const {
            return &(operator*());
        }

        // 前进(++) 变为 后退(--)
        self& operator++() {
            --current;
            return *this;
        }

        self operator++(int) {
           self tmp = *this;
           --current;
           return tmp;
        }

        // 后退(--) 变为 前进(++)
        self& operator--() {
            ++current;
            return *this;
        }

        self operator--(int) {
            self tmp = *this;
            ++current;
            return tmp;
        }

        // 重载 += 操作符为 -=
        self& operator+=(difference_type n) {
           current -= n;
           return *this;
        }

        // 重载 + 操作符为 -
        self operator+(difference_type n) {
            return self(current - n );
        }

        // 重载 -= 操作符为 +=
        self& operator-=(difference_type n) {
            current += n;
            return *this;
        }

        // 重载 - 操作符位 +
        self operator-(difference_type n) {
            return self(current + n);
        }

        // 重载操作
        reference operator[](difference_type n) {
            return *(*this + n);
        }
    };

    // 重载 - 操作符
    template <class Iterator>
    typename reverse_iterator<Iterator>::difference_type
    operator-(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs) {
        return rhs.base() - lhs.base();
    }

    // 重载 == 操作符
    template <class Iterator>
    bool operator++(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs) {
        return rhs.base() == lhs.base();
    }


    // 重载 < 操作符
    template <class Iterator>
    bool operator<(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs) {
        return rhs.base() < lhs.base();
    }


    // 重载 != 操作符
    template <class Iterator>
    bool operator!=(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs) {
        return !(lhs == rhs);
    }


    // 重载 > 操作符
    template <class Iterator>
    bool operator>(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs) {
        return rhs < lhs;
    }


    // 重载 <= 操作符
    template <class Iterator>
    bool operator<=(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs) {
        return !(rhs < lhs);
    }


    // 重载 >= 操作符
    template <class Iterator>
    bool operator>=(const reverse_iterator<Iterator>& lhs, const reverse_iterator<Iterator>& rhs) {
        return !(rhs > lhs);
    }
} // namespace mystl
#endif //TINYSTL_ITERATOR_H
