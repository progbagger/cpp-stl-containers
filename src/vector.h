#ifndef CPP_STL_CONTAINERS_VECTOR_H_
#define CPP_STL_CONTAINERS_VECTOR_H_

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <stdexcept>

namespace cont {

template <class T>
class vector {
 public:
  using value_type = T;
  using reference = T&;
  using const_reference = const T&;
  using pointer = T*;
  using const_pointer = const T*;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using iterator = pointer;
  using const_iterator = const_pointer;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  vector() : data_(nullptr), size_(), capacity_() {}
  explicit vector(size_type size, const_reference value = value_type())
      : data_(allocate(size)), size_(), capacity_(size) {
    for (size_type i = 0; i < capacity_; ++i) push_back(value);
  }
  template <class InputIterator>
  vector(InputIterator first, InputIterator last) : vector() {
    while (first != last) {
      push_back(*first);
      ++first;
    }
  }
  vector(const std::initializer_list<value_type>& list)
      : vector(list.begin(), list.end()) {}

  vector(const vector& other) : vector() { *this = other; }
  vector(vector&& other) noexcept : vector() { *this = std::move(other); }

  vector& operator=(const vector& other) {
    if (this != &other) {
      clear();
      reserve(other.size_);
      for (const auto& i : other) push_back(i);
    }
    return *this;
  }
  vector& operator=(vector&& other) noexcept {
    if (this != &other) {
      std::swap(data_, other.data_);
      std::swap(size_, other.size_);
      std::swap(capacity_, other.capacity_);
    }
    return *this;
  }

  ~vector() {
    for (size_type i = 0; i < size_; ++i) destroy(data_ + i);
    deallocate(data_);
  }

  reference operator[](size_type pos) noexcept { return data_[pos]; }
  const_reference operator[](size_type pos) const noexcept {
    return data_[pos];
  }

  reference at(size_type pos) {
    if (pos >= size_)
      throw std::out_of_range("index " + std::to_string(pos) +
                              " is out of range in vector of size " +
                              std::to_string(size_));
    return data_[pos];
  }
  const_reference at(size_type pos) const {
    if (pos >= size_)
      throw std::out_of_range("index " + std::to_string(pos) +
                              " is out of range in vector of size " +
                              std::to_string(size_));
    return data_[pos];
  }

  iterator begin() noexcept { return data_; }
  const_iterator begin() const noexcept { return cbegin(); }
  const_iterator cbegin() const noexcept { return data_; }

  iterator end() noexcept { return data_ + size_; }
  const_iterator end() const noexcept { return cend(); }
  const_iterator cend() const noexcept { return data_ + size_; }

  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const noexcept { return crbegin(); }
  const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(cend());
  }

  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const noexcept { return crend(); }
  const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(cbegin());
  }

  void push_back(const_reference value) {
    if (size_ == capacity_)
      reserve(capacity_ == size_type() ? 1 : capacity_ * 2);
    new (data_ + size_++) value_type(value);
  }
  void push_back(value_type&& value) {
    if (size_ == capacity_)
      reserve(capacity_ == size_type() ? 1 : capacity_ * 2);
    new (data_ + size_++) value_type(std::move(value));
  }

  void pop_back() {
    if (size_) destroy(data_ + --size_);
  }

  void reserve(size_type size) {
    if (size <= capacity_) return;

    pointer new_data = allocate(size);
    for (size_type i = 0; i < capacity_; ++i) {
      new (new_data + i) value_type(std::move(data_[i]));
      destroy(data_ + i);
    }
    deallocate(data_);
    capacity_ = size;
    data_ = new_data;
  }

  void shrink_to_fit() {
    if (size_ == capacity_) return;
  }

  void clear() {
    for (size_type i = 0; i < size_; ++i) destroy(data_ + i);
    size_ = size_type();
  }

 private:
  static pointer allocate(size_type size) {
    return static_cast<pointer>(::operator new(sizeof(value_type) * size));
  }
  static void deallocate(pointer ptr) {
    ::operator delete(static_cast<void*>(ptr));
  }
  static void destroy(pointer ptr) { ptr->~value_type(); }

  pointer data_;
  size_type size_;
  size_type capacity_;
};

}  // namespace cont

#endif  // CPP_STL_CONTAINERS_VECTOR_H_
