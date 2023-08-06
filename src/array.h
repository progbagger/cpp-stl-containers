#ifndef CPP_STL_CONTAINERS_ARRAY_H_
#define CPP_STL_CONTAINERS_ARRAY_H_

#include <cstddef>
#include <stdexcept>

namespace cont {

template <class T, std::size_t N>
struct array {
 public:
  using value_type = T;
  using reference = value_type &;
  using const_reference = const value_type &;
  using pointer = value_type *;
  using const_pointer = const value_type *;
  using iterator = pointer;
  using const_iterator = const_pointer;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using size_type = std::size_t;

  constexpr reference operator[](size_type pos) noexcept { return values[pos]; }
  constexpr const_reference operator[](size_type pos) const noexcept {
    return values[pos];
  }

  constexpr reference at(size_type pos) {
    if (pos >= N) throw std::out_of_range("oops");
    return values[pos];
  }

  constexpr const_reference at(size_type pos) const {
    if (pos >= N) throw std::out_of_range("oops");
    return values[pos];
  }

  constexpr iterator begin() noexcept { return values; }
  constexpr const_iterator begin() const noexcept { return values; }
  constexpr const_iterator cbegin() const noexcept { return begin(); }

  constexpr reverse_iterator rbegin() noexcept {
    return reverse_iterator(end());
  }
  constexpr const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(end());
  }
  constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }

  constexpr iterator end() noexcept { return values + N; }
  constexpr const_iterator end() const noexcept { return values + N; }
  constexpr const_iterator cend() const noexcept { return end(); }

  constexpr reverse_iterator rend() noexcept {
    return reverse_iterator(begin());
  }
  constexpr const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(begin());
  }
  constexpr const_reverse_iterator crend() const noexcept { return rend(); }

  constexpr reference front() noexcept { return *values; }
  constexpr const_reference front() const noexcept { return *values; }

  constexpr reference back() noexcept { return *(values + N - 1); }
  constexpr const_reference back() const noexcept { return *(values + N - 1); }

  constexpr pointer data() noexcept { return values; }
  constexpr const_pointer data() const noexcept { return values; }

  constexpr size_type size() const noexcept { return N; }
  constexpr size_type max_size() const noexcept { return N; }

  [[nodiscard]] constexpr bool empty() const noexcept { return N == 0; }

  constexpr void swap(array &other) noexcept {
    if (this != &other) std::swap(values, other.values);
  }

  constexpr void fill(const_reference value) {
    std::fill(begin(), end(), value);
  }

  constexpr friend bool operator==(const array &a, const array &b) {
    return std::equal(a.begin(), a.end(), b.begin());
  }

  constexpr friend bool operator!=(const array &a, const array &b) {
    return !(a == b);
  }

  constexpr friend bool operator<(const array &a, const array &b) {
    return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
  }

  constexpr friend bool operator>(const array &a, const array &b) {
    return b < a;
  }

  constexpr friend bool operator<=(const array &a, const array &b) {
    return !(b < a);
  }

  constexpr friend bool operator>=(const array &a, const array &b) {
    return !(a < b);
  }

  value_type values[N];
};

// deduction guides
template <class T, class... U>
array(T, U...) -> array<T, 1 + sizeof...(U)>;

}  // namespace cont

#endif  // CPP_STL_CONTAINERS_ARRAY_H_
