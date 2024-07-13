#ifndef CPP_STL_CONTAINERS_SET_H_
#define CPP_STL_CONTAINERS_SET_H_

#include "avl_tree.h"

namespace cont {

template <class Key, class Compare = std::less<Key>>
class set {
 public:
  using key_type = Key;
  using value_type = key_type;
  using key_compare = Compare;

  struct key_retractor {
    const key_type& operator()(const key_type& key) const { return key; }
  };

  using base = avl_tree<key_type, const Key, key_retractor, key_compare>;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using size_type = typename base::size_type;
  using iterator = typename base::const_iterator;
  using const_iterator = iterator;
  using reverse_iterator = typename base::const_reverse_iterator;
  using const_reverse_iterator = reverse_iterator;

  set() = default;
  set(const set&) = default;
  set(set&&) noexcept = default;

  set& operator=(const set&) = default;
  set& operator=(set&&) noexcept = default;

  iterator begin() const noexcept { return tree_.begin(); }
  iterator cbegin() const noexcept { return tree_.cbegin(); }

  iterator end() const noexcept { return tree_.end(); }
  iterator cend() const noexcept { return tree_.cend(); }

  reverse_iterator rbegin() const noexcept { return tree_.rbegin(); }
  reverse_iterator crbegin() const noexcept { return tree_.crbegin(); }

  reverse_iterator rend() const noexcept { return tree_.rend(); }
  reverse_iterator crend() const noexcept { return tree_.crend(); }

  [[nodiscard]] bool empty() const noexcept { return tree_.empty(); }
  size_type size() const noexcept { return tree_.size(); }
  size_type max_size() const noexcept { return tree_.max_size(); }

  void clear() { tree_.clear(); }

  std::pair<iterator, bool> insert(const value_type& value) {
    return tree_.insert(value);
  }

  std::pair<iterator, bool> insert(value_type&& value) {
    return tree_.insert(std::move(value));
  }

 private:
  base tree_;
};

}  // namespace cont

#endif  // CPP_STL_CONTAINERS_SET_H_
