#pragma once

#include <cassert>
#include <initializer_list>
#include <stdexcept>

#include "array_ptr.h"

class ReserveProxyObj
{
public:
    ReserveProxyObj(size_t capacity)
        : capacity_(capacity)
    {
    }

    size_t GetCapacity()
    {
        return capacity_;
    }

private:
    size_t capacity_;
};

template <typename Type>
class SimpleVector
{
public:
    using Iterator = Type *;
    using ConstIterator = const Type *;

    SimpleVector() noexcept = default;

    explicit SimpleVector(size_t size) : SimpleVector(size, Type{}) {}

    SimpleVector(size_t size, const Type &value) : arr_(size), size_(size), capacity_(size)
    {
        std::fill(begin(), end(), value);
    }

    SimpleVector(std::initializer_list<Type> init) : arr_(init.size()), size_(init.size()), capacity_(init.size())
    {
        std::copy(init.begin(), init.end(), begin());
    }

    SimpleVector(const SimpleVector &other) : arr_(other.capacity_), size_(other.size_), capacity_(other.capacity_)
    {
        std::copy(other.begin(), other.end(), begin());
    }

    SimpleVector(SimpleVector &&other)
    {
        swap(other);
    }

    SimpleVector(ReserveProxyObj obj)
    {
        Reserve(obj.GetCapacity());
    }

    SimpleVector &operator=(const SimpleVector &other)
    {
        if (&arr_ != &other.arr_)
        {
            ArrayPtr<Type> temp(other.GetCapacity());
            std::copy(other.begin(), other.end(), temp.Get());
            arr_.swap(temp);
            size_ = other.GetSize();
            capacity_ = other.GetCapacity();
        }
        return *this;
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type &item)
    {
        if (size_ + 1 > capacity_)
        {
            size_t new_capacity = std::max(size_ + 1, capacity_ * 2);
            ArrayPtr<Type> temp(new_capacity);
            std::fill(temp.Get(), temp.Get() + new_capacity, Type{});
            std::copy(arr_.Get(), arr_.Get() + size_, temp.Get());
            arr_.swap(temp);
            capacity_ = new_capacity;
        }
        arr_[size_] = item;
        ++size_;
    }

    void PushBack(Type &&item)
    {
        if (size_ + 1 > capacity_)
        {
            size_t new_capacity = std::max(size_ + 1, capacity_ * 2);
            ArrayPtr<Type> temp(new_capacity);
            // std::fill(temp.Get(), temp.Get() + new_capacity, Type{});
            // std::copy(arr_.Get(), arr_.Get() + size_, temp.Get());
            std::move(arr_.Get(), arr_.Get() + size_, temp.Get());
            arr_.swap(temp);
            capacity_ = new_capacity;
        }
        arr_[size_] = std::move(item);
        ++size_;
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type &value)
    {

        // Напишите тело самостоятельно
        if (capacity_ == 0) // 1 2 (42) 3 4
        {
            capacity_ = 1;
        }
        else if (size_ + 1 > capacity_)
        {
            capacity_ *= 2;
        }
        ArrayPtr<Type> tmp(capacity_);
        auto it = pos - begin();
        std::copy(begin(), begin() + it, tmp.Get());
        *(tmp.Get() + it) = value;

        std::copy(begin() + it, end(), tmp.Get() + it + 1);
        ++size_;
        arr_.swap(tmp);

        return begin() + it;
    }

    Iterator Insert(ConstIterator pos, Type &&value)
    {

        // Напишите тело самостоятельно
        if (capacity_ == 0) // 1 2 (42) 3 4
        {
            capacity_ = 1;
        }
        else if (size_ + 1 > capacity_)
        {
            capacity_ *= 2;
        }
        ArrayPtr<Type> tmp(capacity_);
        auto it = pos - begin();
        std::move(begin(), begin() + it, tmp.Get());
        *(tmp.Get() + it) = std::move(value);

        std::move(begin() + it, end(), tmp.Get() + it + 1);
        ++size_;
        arr_.swap(tmp);

        return begin() + it;
    }

    void PopBack() noexcept
    {
        if (size_ != 0)
        {
            --size_;
        }
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos)
    {

        if (size_ != 0)
        {
            Iterator it = begin() + (pos - begin());

            std::move(it + 1, end(), it);

            --size_;

            return it;
        }

        return end();
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector &other) noexcept
    {
        std::swap(other.capacity_, capacity_);
        std::swap(other.size_, size_);
        arr_.swap(other.arr_);
    }

    void Reserve(size_t new_capacity)
    {
        if (new_capacity > capacity_)
        {
            ArrayPtr<Type> temp(new_capacity);
            std::fill(temp.Get(), temp.Get() + new_capacity, Type{});
            std::copy(arr_.Get(), arr_.Get() + size_, temp.Get());
            arr_.swap(temp);
            capacity_ = new_capacity;
        }
    }

    size_t GetSize() const noexcept
    {
        return size_;
    }

    size_t GetCapacity() const noexcept
    {
        return capacity_;
    }

    bool IsEmpty() const noexcept
    {
        return (size_ == 0);
    }

    Type &operator[](size_t index) noexcept
    {
        return arr_[index];
    }

    const Type &operator[](size_t index) const noexcept
    {
        return arr_[index];
    }

    Type &At(size_t index)
    {
        if (index >= size_)
        {
            throw std::out_of_range("out of range");
        }
        return arr_[index];
    }

    const Type &At(size_t index) const
    {
        if (index >= size_)
        {
            throw std::out_of_range("out of range");
        }
        return arr_[index];
    }

    void Clear() noexcept
    {
        size_ = 0;
    }

    void Resize(size_t new_size)
    {
        if (new_size < size_)
        {
            size_ = new_size;
        }
        else if (new_size < capacity_)
        {
            for (auto it = end(); it != begin() + new_size; it++)
            {
                *it = std::move(Type{});
            }
            size_ = new_size;
        }
        else
        {
            ArrayPtr<Type> temp(new_size * 2);
            std::move(begin(), end(), &temp[0]);
            arr_.swap(temp);
            size_ = new_size;
            capacity_ = new_size * 2;
        }
    }

    Iterator begin() noexcept
    {
        return arr_.Get();
    }

    Iterator end() noexcept
    {
        return arr_.Get() + size_;
    }

    ConstIterator begin() const noexcept
    {
        return arr_.Get();
    }

    ConstIterator end() const noexcept
    {
        return arr_.Get() + size_;
    }

    ConstIterator cbegin() const noexcept
    {
        return arr_.Get();
    }

    ConstIterator cend() const noexcept
    {
        return arr_.Get() + size_;
    }

private:
    ArrayPtr<Type> arr_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs)
{
    return std::equal(lhs.begin(), lhs.end(),
                      rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs)
{
    return !std::equal(lhs.begin(), lhs.end(),
                       rhs.begin());
}

template <typename Type>
inline bool operator<(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                        rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs)
{
    return (lhs < rhs || lhs == rhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs)
{
    return !(lhs <= rhs);
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs)
{
    return !(lhs < rhs);
}

ReserveProxyObj Reserve(size_t capacity_to_reserve)
{
    return ReserveProxyObj(capacity_to_reserve);
}