#ifndef CPP_STL_CONTAINERS_QUEUE_H_
#define CPP_STL_CONTAINERS_QUEUE_H_

#include "list.h"

namespace cont {

template <class T, class Container = list<T>>
class queue {
 public:
  using container_type = Container;
  using value_type = typename container_type::value_type;
  using reference = typename container_type::reference;
  using const_reference = typename container_type::const_reference;
  using size_type = typename container_type::size_type;

  explicit queue(const container_type& cont = container_type()) : cont_(cont) {}
  explicit queue(container_type&& cont) : cont_(std::move(cont)) {}

  template <class InputIt>
  queue(InputIt first, InputIt last) : queue() {
    while (first != last) {
      ;
    }
  }

  reference front() { return cont_.front(); }
  const_reference front() const { return cont_.front(); }

  reference back() { return cont_.back(); }
  const_reference back() const { return cont_.back(); }

  [[nodiscard]] bool empty() const noexcept { return cont_.empty(); }
  size_type size() const noexcept { return cont_.size(); }

  void push(const_reference value) { cont_.push_back(value); }
  void push(value_type&& value) { cont_.push_back(std::move(value)); }

  template <class... Args>
  void emplace(Args&&... args) {
    cont_.emplace_back(std::forward<Args>(args)...);
  }

  void pop() { cont_.pop_front(); }

  void swap(queue& other) { cont_.swap(other.cont_); }

  friend bool operator==(const queue& l, const queue& r) {
    return l.cont_ == r.cont_;
  }

  friend bool operator!=(const queue& l, const queue& r) {
    return l.cont_ != r.cont_;
  }

  friend bool operator<(const queue& l, const queue& r) {
    return l.cont_ < r.cont_;
  }

  friend bool operator<=(const queue& l, const queue& r) {
    return l.cont_ <= r.cont_;
  }

  friend bool operator>(const queue& l, const queue& r) {
    return l.cont_ > r.cont_;
  }

  friend bool operator>=(const queue& l, const queue& r) {
    return l.cont_ >= r.cont_;
  }

 protected:
  Container cont_;
};

// deduction guides

template <class Container>
queue(Container) -> queue<typename Container::value_type, Container>;

template <class InputIt>
queue(InputIt, InputIt)
    -> queue<typename std::iterator_traits<InputIt>::value_type>;

}  // namespace cont

#endif  // CPP_STL_CONTAINERS_QUEUE_H_
