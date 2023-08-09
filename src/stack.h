#ifndef CPP_STL_CONTAINERS_H_
#define CPP_STL_CONTAINERS_H_

#include "list.h"

namespace cont {

template <class T, class Container = list<T>>
class stack {
 public:
  using container_type = Container;
  using value_type = typename container_type::value_type;
  using reference = typename container_type::reference;
  using const_reference = typename container_type::const_reference;
  using size_type = typename container_type::size_type;

  explicit stack(const container_type& cont = container_type()) : cont_(cont) {}
  explicit stack(Container&& cont) : cont_(std::move(cont)) {}

  template <class InputIt>
  stack(InputIt first, InputIt last) : stack() {
    while (first != last) {
      push(*first);
      ++first;
    }
  }

  reference top() { return cont_.back(); }
  const_reference top() const { return cont_.back(); }

  [[nodiscard]] bool empty() const noexcept { return cont_.empty(); }
  size_type size() const noexcept { return cont_.size(); }

  void push(const_reference value) { cont_.push_back(value); }
  void push(value_type&& value) { cont_.push_back(std::move(value)); }

  template <class... Args>
  void emplace(Args&&... args) {
    cont_.emplace_back(std::forward<Args>(args)...);
  }

  void pop() { cont_.pop_back(); }

  void swap(stack& other) noexcept { cont_.swap(other.cont_); }

  friend bool operator==(const stack& s1, const stack& s2) {
    return s1.cont_ == s2.cont_;
  }

  friend bool operator!=(const stack& s1, const stack& s2) {
    return s1.cont_ != s2.cont_;
  }

  friend bool operator<(const stack& s1, const stack& s2) {
    return s1.cont_ < s2.cont_;
  }

  friend bool operator<=(const stack& s1, const stack& s2) {
    return s1.cont_ <= s2.cont_;
  }

  friend bool operator>(const stack& s1, const stack& s2) {
    return s1.cont_ > s2.cont_;
  }

  friend bool operator>=(const stack& s1, const stack& s2) {
    return s1.cont_ >= s2.cont_;
  }

 protected:
  Container cont_;
};

// deduction guides

template <class Container>
stack(Container) -> stack<typename Container::value_type, Container>;

template <class InputIt>
stack(InputIt, InputIt)
    -> stack<typename std::allocator_traits<InputIt>::value_type>;

}  // namespace cont

#endif  // CPP_STL_CONTAINERS_H_
