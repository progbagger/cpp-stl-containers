#ifndef CPP_STL_CONTAINERS_MULTIMAP_H_
#define CPP_STL_CONTAINERS_MULTIMAP_H_

#include <iterator>

#include "avl_tree.h"

namespace cont {

template <class Key, class T, class Compare = std::less<Key>>
class multimap {
 public:
  using key_type = Key;
  using mapped_type = T;
  using key_compare = Compare;
  using value_type = std::pair<const key_type, mapped_type>;

  struct key_retractor {
   public:
    const key_type& operator()(const value_type& p) const { return p.first; }
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

  multimap() = default;
  multimap(const multimap&) = default;
  multimap(multimap&&) noexcept = default;
  multimap& operator=(const multimap&) = default;
  multimap& operator=(multimap&&) noexcept = default;

  explicit multimap(const std::initializer_list<value_type>& items)
      : multimap(items.begin(), items.end()) {
    insert(items);
  }
  template <class InputIterator>
  explicit multimap(InputIterator first, InputIterator last) : multimap() {
    insert(first, last);
  }

  multimap& operator=(const std::initializer_list<value_type>& items) {
    *this = std::move(multimap(items));
    return *this;
  }

  ~multimap() = default;

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
    return tree_.insert(value, true);
  }
  std::pair<iterator, bool> insert(value_type&& value) {
    return tree_.insert(std::move(value), true);
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

  void swap(multimap& other) noexcept { tree_.swap(other.tree_); }
  void merge(multimap& other) noexcept { tree_.merge(other.tree_); }

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

  friend bool operator==(const multimap& m1, const multimap& m2) {
    return m1.tree_ == m2.tree_;
  }
  friend bool operator!=(const multimap& m1, const multimap& m2) {
    return m1.tree_ != m2.tree_;
  }
  friend bool operator<(const multimap& m1, const multimap& m2) {
    return m1.tree_ < m2.tree_;
  }
  friend bool operator<=(const multimap& m1, const multimap& m2) {
    return m1.tree_ <= m2.tree_;
  }
  friend bool operator>(const multimap& m1, const multimap& m2) {
    return m1.tree_ > m2.tree_;
  }
  friend bool operator>=(const multimap& m1, const multimap& m2) {
    return m1.tree_ >= m2.tree_;
  }

 private:
  base tree_;
};

}  // namespace cont

#endif  // CPP_STL_CONTAINERS_MULTIMAP_H_
