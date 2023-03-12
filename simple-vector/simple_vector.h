#include "array_ptr.h"
#include <stdexcept>
#include <algorithm>
#include <initializer_list>
#include <iostream>
#include  <utility>
#include <cassert>

class ReserveProxyObj {
public:    
    explicit ReserveProxyObj (size_t capacity_to_reserve)
        :capacity_(capacity_to_reserve)
    {    
    }
       
    size_t ReserveCapasity(){
        return capacity_;
    }
    
private:    
    size_t capacity_;
 
};
 
ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    explicit SimpleVector(size_t size) : elements_(size), size_(size), capacity_(size) {
        for(auto it = begin(); it != end(); ++it) {
            *it = Type();
        }
    }
    
    SimpleVector(size_t size, const Type& value) : elements_(size), size_(size), capacity_(size) {
       std::fill(begin(), end(), value);
    }

   SimpleVector(std::initializer_list<Type> init) : elements_(init.size()), size_(init.size()),  capacity_ (init.size()) {
        std::copy(init.begin(), init.end(), begin());
    }
    
    
    SimpleVector(const SimpleVector& other) : elements_(other.size_), size_(other.size_), capacity_(other.size_) {
        std::copy(other.begin(), other.end(), begin());
    }
    
    
    SimpleVector& operator=(const SimpleVector& rhs) {
        if(&rhs == this) {
            return *this;
        }
        SimpleVector<Type> temp(rhs);
        swap(temp);
        return *this;
    }
    
     SimpleVector& operator=(SimpleVector&& rhs) {
        if(this != &rhs){
           elements_.swap(rhs.elements_);
           size_ = std::move(rhs.size_);
           capacity_ = std::move(rhs.capacity_);
        }
        return *this;
    }
    
    SimpleVector( ReserveProxyObj capacity_to_reserve) {
        Reserve(capacity_to_reserve.ReserveCapasity() );
    }
    
   SimpleVector(SimpleVector&& other) {
       elements_ = std::move(other.elements_);
       std::swap(size_, other.size_);
       std::swap(capacity_, other.capacity_);
    }

    size_t GetSize() const noexcept {
        return size_;
    }

    size_t GetCapacity() const noexcept {
       return capacity_;
    }

    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    Type& operator[](size_t index) noexcept {
        return elements_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        assert(index > size_);
        return elements_[index];
    }

    Type& At(size_t index) {
        assert(index > size_);
        return elements_[index];
    }

    const Type& At(size_t index) const {
         if (index >= GetSize()) {
            throw std::out_of_range("Error: incorrect index");
        }
        return elements_[index];
    }

    void Clear() noexcept {
        size_ = 0;
    }

    void Resize(size_t new_size) {
        if (new_size <= size_) {
            size_ = new_size;
            return;
        }
        if (new_size <= capacity_) {
            for(auto it = begin() + size_; it != begin() + new_size; ++it) {
                *it = Type();
            }
            size_ = new_size;
            return;
        }
        
        SimpleVector<Type> temp(new_size);
        std::move(begin(),end(), temp.begin());
        swap(temp);
    }

    void PushBack(const Type& item) {
        if(size_ < capacity_) {
            size_++;
            elements_[size_-1] = item;
            return;
        }
        if (size_ == 0) {
           Resize(1);
           elements_[size_-1] = item;
           return;
        } 
        auto tmpsize = size_;
        Resize( capacity_ << 1);
        size_ = tmpsize + 1;
        elements_[tmpsize] = item;
    }
    
    void PushBack(Type&& item) {
        if(size_ < capacity_) {
            size_++;
            elements_[size_-1] = std::move(item);
            return;
        }
        if (size_ == 0) {
           Resize(1);
           elements_[size_-1] = std::move(item);
           return;
        } 
        auto tmpsize = size_;
        Resize( capacity_ << 1);
        size_ = tmpsize + 1;
        elements_[tmpsize] = std::move(item);
    }
    
    
    Iterator Insert(ConstIterator pos, const Type& value) {
        if (pos >= begin() && pos < end()) {
            auto dist = std::distance(cbegin(), pos);
            if(dist < 0 || dist > (long int)size_) {
                throw std::out_of_range("Error: incorrect pos");
            }
            if (IsEmpty()){
                PushBack(value);
                return begin();
            }

            if (size_ == capacity_) {
                SimpleVector<Type> temp(size_ << 1);
                std::copy(begin(), begin() + dist, temp.begin());
                temp[dist] = value;
                std::copy(begin() + (dist), end(), temp.begin() + (dist + 1));
                temp.size_ = size_ + 1;
                swap(temp);
                return (begin() + dist);
            }

            std::copy_backward((Iterator)pos, end(), end() + 1);
            elements_[dist] = value;
            ++size_;
            return (begin() + dist);
        }
        return end();
    }
    
    Iterator Insert(ConstIterator pos, Type&& value) {
            auto dist = std::distance(cbegin(), pos);
            if(dist < 0 || dist > (long int)size_) {
                throw std::out_of_range("Error: incorrect pos");
            }
            if (IsEmpty()){
                PushBack(std::move(value));
                return begin();
            }

            if (size_ == capacity_) {
                SimpleVector<Type> temp(size_ << 1);
                std::move(begin(), begin() + dist, temp.begin());
                temp[dist] = std::move(value);
                std::move(begin() + (dist), end(), temp.begin() + (dist + 1));
                temp.size_ = size_ + 1;
                swap(temp);
                return (begin() + dist);
            }

            std::move_backward((Iterator)pos, end(), end() + 1);
            elements_[dist] = std::move(value);
            ++size_;
            return (begin() + dist);
    }
    
    
    void PopBack() noexcept {
        assert(IsEmpty());
        --size_;
    }
    
    Iterator Erase(ConstIterator pos) {
    if (pos >= begin() && pos < end()) {
        std::move((Iterator)pos + 1, end(), (Iterator)pos);
        --size_;
        return (Iterator)pos;
    }
    return end();
    }
    
    void swap(SimpleVector& other) noexcept {
       elements_.swap(other.elements_);
       std::swap(size_, other.size_);
       std::swap(capacity_, other.capacity_);
    }
    
    
    void Reserve(size_t new_capacity) {
        if (new_capacity <= capacity_) {
            return;
        }
        
        ArrayPtr<Type> temp_elements(new_capacity);
        for (std::size_t i{}; i < size_; ++i){
            temp_elements[i] = elements_[i];
        }
            elements_.swap(temp_elements);
            capacity_ = new_capacity;
    }


    Iterator begin() noexcept {
        return elements_.Get();
    }


    Iterator end() noexcept {
        return (elements_.Get() + size_);
    }

    ConstIterator begin() const noexcept {
        return &elements_[0];
    }

    ConstIterator end() const noexcept {
        return &elements_[size_];
    }


    ConstIterator cbegin() const noexcept {
        return &elements_[0];
    }


    ConstIterator cend() const noexcept {
        return &elements_[size_];
    }
    
    private:
    ArrayPtr<Type> elements_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
     return (lhs.GetSize() == rhs.GetSize()) && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (lhs < rhs || lhs == rhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
} 
