#ifndef CPP_STL_CONTAINERS_MAP_H_
#define CPP_STL_CONTAINERS_MAP_H_

#include "avl_tree.h"

namespace cont {

template <class Key, class T, class Compare = std::less<Key>>
class map {
 public:
  using key_type = Key;
  using mapped_type = T;
  using key_compare = Compare;
  using value_type = std::pair<const key_type, mapped_type>;

  struct key_retractor {
   public:
    const key_type& operator()(const value_type& p) const { return p.first; }
  };

  using base = avl_tree<Key, value_type, key_retractor, key_compare>;
  using reference = typename base::reference;
  using const_reference = typename base::const_reference;
  using size_type = typename base::size_type;
  using difference_type = typename base::difference_type;
  using iterator = typename base::iterator;
  using const_iterator = typename base::const_iterator;
  using reverse_iterator = typename base::reverse_iterator;
  using const_reverse_iterator = typename base::const_reverse_iterator;

  map() = default;
  map(const map&) = default;
  map(map&&) noexcept = default;
  map& operator=(const map&) = default;
  map& operator=(map&&) noexcept = default;

  explicit map(const std::initializer_list<value_type>& items) : tree_() {
    insert(items);
  }
  template <class InputIterator>
  map(InputIterator first, InputIterator last) : tree_() {
    insert(first, last);
  }

  map& operator=(const std::initializer_list<value_type>& items) {
    *this = std::move(map(items));
    return *this;
  }

  ~map() = default;

  mapped_type& at(const key_type& key) { return tree_.at(key).second; }
  const mapped_type& at(const key_type& key) const {
    return tree_.at(key).second;
  }

  mapped_type& operator[](const key_type& key) {
    return tree_.insert({key, mapped_type()}).first->second;
  }
  mapped_type& operator[](key_type&& key) {
    return tree_.insert({std::move(key), mapped_type()}).first->second;
  }

  iterator begin() noexcept { return tree_.begin(); }
  const_iterator begin() const noexcept { return tree_.begin(); }
  const_iterator cbegin() const noexcept { return tree_.cbegin(); }

  iterator end() noexcept { return tree_.end(); }
  const_iterator end() const noexcept { return tree_.end(); }
  const_iterator cend() const noexcept { return tree_.cend(); }

  reverse_iterator rbegin() noexcept { return tree_.rbegin(); }
  const_reverse_iterator rbegin() const noexcept { return tree_.rbegin(); }
  const_reverse_iterator crbegin() const noexcept { return tree_.crbegin(); }

  reverse_iterator rend() noexcept { return tree_.rend(); }
  const_reverse_iterator rend() const noexcept { return tree_.rend(); }
  const_reverse_iterator crend() const noexcept { return tree_.crend(); }

  bool empty() const noexcept { return tree_.empty(); }
  size_type size() const noexcept { return tree_.size(); }
  size_type max_size() const noexcept { return tree_.max_size(); }

  void clear() noexcept { tree_.clear(); }

  std::pair<iterator, bool> insert(const value_type& value) {
    return tree_.insert(value, false);
  }
  std::pair<iterator, bool> insert(value_type&& value) {
    return tree_.insert(std::move(value), false);
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

  template <class ValueType>
  std::pair<iterator, bool> insert_or_assign(const key_type& key,
                                             ValueType&& value) {
    auto pos = find(key);
    if (pos == end()) return insert({key, std::forward<ValueType>(value)});
    pos->second = std::forward<ValueType>(value);
    return {pos, false};
  }
  template <class ValueType>
  std::pair<iterator, bool> insert_or_assign(key_type&& key,
                                             ValueType&& value) {
    auto pos = find(key);
    if (pos == end())
      return insert({std::move(key), std::forward<ValueType>(value)});
    pos->second = std::forward<ValueType>(value);
    return {pos, false};
  }

  iterator erase(const_iterator pos) { return tree_.erase(pos); }
  size_type erase(const key_type& key) { return tree_.erase(key); }

  void swap(map& other) { tree_.swap(other.tree_); }
  void merge(map& other) { tree_.merge(other.tree_); }

  iterator find(const key_type& key) { return tree_.find(key); }
  const_iterator find(const key_type& key) const { return tree_.find(key); }

  bool contains(const key_type& key) const { return tree_.contains(key); }

  std::pair<iterator, iterator> equal_range(const key_type& key) {
    return tree_.equal_range(key);
  }
  std::pair<const_iterator, const_iterator> equal_range(
      const key_type& key) const {
    return tree_.equal_range(key);
  }

  iterator lower_bound(const key_type& key) { return tree_.lower_bound(key); }
  const_iterator lower_bound(const key_type& key) const {
    return tree_.lower_bound(key);
  }

  iterator upper_bound(const key_type& key) { return tree_.upper_bound(key); }
  const_iterator upper_bound(const key_type& key) const {
    return tree_.upper_bound(key);
  }

  friend bool operator==(const map& m1, const map& m2) {
    return m1.tree_ == m2.tree_;
  }
  friend bool operator!=(const map& m1, const map& m2) {
    return m1.tree_ != m2.tree_;
  }
  friend bool operator<(const map& m1, const map& m2) {
    return m1.tree_ < m2.tree_;
  }
  friend bool operator<=(const map& m1, const map& m2) {
    return m1.tree_ <= m2.tree_;
  }
  friend bool operator>(const map& m1, const map& m2) {
    return m1.tree_ > m2.tree_;
  }
  friend bool operator>=(const map& m1, const map& m2) {
    return m1.tree_ >= m2.tree_;
  }

 private:
  base tree_;
};

}  // namespace cont

#endif  // CPP_STL_CONTAINERS_MAP_H_
