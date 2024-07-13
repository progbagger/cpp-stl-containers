#ifndef CPP_STL_CONTAINERS_MULTISET_H_
#define CPP_STL_CONTAINERS_MULTISET_H_

#include <initializer_list>

#include "avl_tree.h"

namespace cont {

template <class T, class Compare = std::less<T>>
class multiset {
 public:
  using key_type = const T;
  using value_type = key_type;
  using key_compare = Compare;

  struct key_retractor {
   public:
    const key_type& operator()(const value_type& t) const { return t; }
  };

  using base = avl_tree<key_type, value_type, key_retractor, key_compare>;
  using reference = typename base::reference;
  using const_reference = typename base::const_reference;
  using size_type = typename base::size_type;
  using difference_type = typename base::difference_type;
  using iterator = typename base::iterator;
  using const_iterator = typename base::const_iterator;
  using reverse_iterator = typename base::reverse_iterator;
  using const_reverse_iterator = typename base::const_reverse_iterator;

  multiset() = default;
  multiset(const multiset&) = default;
  multiset(multiset&&) noexcept = default;
  multiset& operator=(const multiset&) = default;
  multiset& operator=(multiset&&) noexcept = default;

  ~multiset() = default;

  explicit multiset(const std::initializer_list<value_type>& items)
      : multiset(items.begin(), items.end()) {}
  template <class InputIterator>
  explicit multiset(InputIterator first, InputIterator last) : multiset() {
    insert(first, last);
  }

  multiset& operator=(const std::initializer_list<value_type>& items) {
    *this = std::move(multiset(items));
    return *this;
  }

  iterator begin() const noexcept { return tree_.begin(); }
  iterator cbegin() const noexcept { return tree_.cbegin(); }

  iterator end() const noexcept { return tree_.end(); }
  iterator cend() const noexcept { return tree_.cend(); }

  reverse_iterator rbegin() const noexcept { return tree_.rbegin(); }
  reverse_iterator crbegin() const noexcept { return tree_.crbegin(); }

  reverse_iterator rend() const noexcept { return tree_.rend(); }
  reverse_iterator crend() const noexcept { return tree_.crend(); }

  bool empty() const noexcept { return tree_.empty(); }
  size_type size() const noexcept { return tree_.size(); }
  size_type max_size() const noexcept { return tree_.max_size(); }

  void clear() noexcept { return tree_.clear(); }

  iterator insert(const value_type& value) {
    return tree_.insert(value, true).first;
  }
  iterator insert(value_type&& value) {
    return tree_.insert(std::move(value), true).first;
  }
  void insert(const std::initializer_list<value_type>& items) {
    insert(items.begin(), items.end());
  }
  template <class InputIterator>
  void insert(InputIterator first, InputIterator last) {
    while (first != last) {
      insert(*first);
      ++first;
    }
  }

  iterator erase(const_iterator pos) { return tree_.erase(pos); }
  size_type erase(const key_type& key) { return tree_.erase(key); }

  void swap(multiset& other) { tree_.swap(other.tree_); }
  void merge(multiset& other) { tree_.merge(other.tree_); }

  iterator find(const key_type& key) { return tree_.find(key); }

  bool contains(const key_type& key) const { return tree_.contains(key); }

  std::pair<iterator, iterator> equal_range(const key_type& key) const {
    return tree_.equal_range(key);
  }

  iterator upper_bound(const key_type& key) const {
    return tree_.upper_bound(key);
  }

  friend bool operator==(const multiset& m1, const multiset& m2) {
    return m1.tree_ == m2.tree_;
  }
  friend bool operator!=(const multiset& m1, const multiset& m2) {
    return m1.tree_ != m2.tree_;
  }
  friend bool operator<(const multiset& m1, const multiset& m2) {
    return m1.tree_ < m2.tree_;
  }
  friend bool operator<=(const multiset& m1, const multiset& m2) {
    return m1.tree_ <= m2.tree_;
  }
  friend bool operator>(const multiset& m1, const multiset& m2) {
    return m1.tree_ > m2.tree_;
  }
  friend bool operator>=(const multiset& m1, const multiset& m2) {
    return m1.tree_ >= m2.tree_;
  }

 private:
  base tree_;
};

}  // namespace cont

#endif  // CPP_STL_CONTAINERS_MULTISET_H_
