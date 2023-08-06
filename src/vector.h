#ifndef CPP_STL_CONTAINERS_VECTOR_H_
#define CPP_STL_CONTAINERS_VECTOR_H_

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <limits>
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
    swap(other);
    return *this;
  }
  vector& operator=(const std::initializer_list<value_type>& list) {
    *this = vector(list);
    return *this;
  }

  ~vector() {
    clear();
    deallocate(data_);
  }

  void assign(size_type count, const_reference value) {
    clear();
    reserve(count);
    while (count--) push_back(value);
  }

  template <class InputIterator>
  void assign(InputIterator first, InputIterator last) {
    clear();
    while (first != last) {
      push_back(*first);
      ++first;
    }
  }

  void assign(const std::initializer_list<value_type>& list) {
    clear();
    reserve(list.size());
    assign(list.begin(), list.end());
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

  reference operator[](size_type pos) noexcept { return data_[pos]; }
  const_reference operator[](size_type pos) const noexcept {
    return data_[pos];
  }

  reference front() { return *data_; }
  const_reference front() const { return *data_; }

  reference back() { return data_[size_ - 1]; }
  const_reference back() const { return data_[size_ - 1]; }

  pointer data() { return data_; }
  const_pointer data() const { return data_; }

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

  [[nodiscard]] bool empty() const noexcept { return !size_; }
  size_type size() const noexcept { return size_; }
  size_type max_size() const noexcept {
    return std::numeric_limits<difference_type>::max() / sizeof(value_type);
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

  size_type capacity() const noexcept { return capacity_; }

  void shrink_to_fit() {
    if (size_ == capacity_) return;

    pointer new_data = allocate(size_);
    for (size_type i = 0; i < size_; ++i) {
      new (new_data + i) value_type(std::move(data_[i]));
      destroy(data_ + i);
    }
    deallocate(data_);
    data_ = new_data;
  }

  void clear() {
    for (size_type i = 0; i < size_; ++i) destroy(data_ + i);
    size_ = size_type();
  }

  iterator insert(const_iterator pos, const_reference value) {
    size_type distance = pos - begin();
    push_back(value);
    iterator from = begin() + distance;
    while (from != end()) std::swap(*(from++), back());
    return begin() + distance;
  }

  iterator insert(const_iterator pos, value_type&& value) {
    size_type distance = pos - begin();
    push_back(std::move(value));
    iterator from = begin() + distance;
    while (from != end()) std::swap(*(from++), back());
    return begin() + distance;
  }

  iterator insert(const_iterator pos, size_type count, const_reference value) {
    size_type distance = pos - begin();
    reserve(size_ + count);
    iterator result = begin() + distance;
    while (count--) result = insert(result, value);
    return result;
  }

  template <class InputIterator>
  iterator insert(const_iterator pos, InputIterator first, InputIterator last) {
    iterator result = pos;
    while (first != last) {
      result = insert(result, *first);
      ++first;
    }
    return result;
  }

  iterator insert(const_iterator pos,
                  const std::initializer_list<value_type>& list) {
    return insert(pos, list.begin(), list.end());
  }

  iterator erase(const_iterator pos) {
    iterator result = pos + 1;
    while (pos != --end()) std::swap(*pos, *(++pos));
    pop_back();
    return result;
  }

  iterator erase(const_iterator first, const_iterator last) {
    iterator result = first;
    while (first != last) result = erase(first);
    return result;
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

  void resize(size_type count, const_reference value = value_type()) {
    if (size_ == count) return;

    reserve(count);
    while (size_ < count) push_back(value);
  }

  void swap(vector& other) noexcept {
    if (this != &other) {
      std::swap(data_, other.data_);
      std::swap(size_, other.size_);
      std::swap(capacity_, other.capacity_);
    }
  }

  friend bool operator==(const vector& v1, const vector& v2) {
    return v1.size() == v2.size() &&
           std::equal(v1.begin(), v1.end(), v2.begin(), v2.end());
  }

  friend bool operator!=(const vector& v1, const vector& v2) {
    return !(v1 == v2);
  }

  friend bool operator<(const vector& v1, const vector& v2) {
    return std::lexicographical_compare(v1.begin(), v1.end(), v2.begin(),
                                        v2.end());
  }

  friend bool operator<=(const vector& v1, const vector& v2) {
    return !(v1 > v2);
  }

  friend bool operator>(const vector& v1, const vector& v2) { return v2 < v1; }

  friend bool operator>=(const vector& v1, const vector& v2) {
    return !(v1 < v2);
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

// deduction guides

template <class InputIterator>
vector(InputIterator, InputIterator)
    -> vector<typename std::iterator_traits<InputIterator>::value_type>;

}  // namespace cont

#endif  // CPP_STL_CONTAINERS_VECTOR_H_
