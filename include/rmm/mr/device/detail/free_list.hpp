/*
 * Copyright (c) 2019-2020, NVIDIA CORPORATION.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <algorithm>
#include <iostream>
#include <list>

namespace rmm {
namespace mr {
namespace detail {

/**
 * @brief An ordered list of free memory blocks that coalesces contiguous blocks on insertion.
 *
 * @tparam list_type the type of the internal list data structure.
 */
template <typename block_t = block, typename list_t = std::list<block_t>>
struct free_list {
  free_list()  = default;
  ~free_list() = default;

  using size_type      = typename list_t::size_type;
  using iterator       = typename list_t::iterator;
  using const_iterator = typename list_t::const_iterator;

  iterator begin() noexcept { return blocks.begin(); }                /// beginning of the free list
  const_iterator begin() const noexcept { return begin(); }           /// beginning of the free list
  const_iterator cbegin() const noexcept { return blocks.cbegin(); }  /// beginning of the free list

  iterator end() noexcept { return blocks.end(); }                /// end of the free list
  const_iterator end() const noexcept { return end(); }           /// end of the free list
  const_iterator cend() const noexcept { return blocks.cend(); }  /// end of the free list

  /**
   * @brief The size of the free list in blocks.
   *
   * @return size_type The number of blocks in the free list.
   */
  size_type size() const noexcept { return blocks.size(); }

  /**
   * @brief checks whether the free_list is empty.
   *
   * @return true If there are blocks in the free_list.
   * @return false If there are no blocks in the free_list.
   */
  bool is_empty() const noexcept { return blocks.empty(); }

  /**
   * @brief Inserts a block into the `free_list` in the correct order, coalescing it with the
   *        preceding and following blocks if either is contiguous.
   *
   * @param b The block to insert.
   */
  void insert(block_t b) { coalesced_emplace(std::move(b)); }

  template <typename... Args>
  void coalesced_emplace(Args&&... args)
  {
    block_t b(std::forward<Args>(args)...);

    if (is_empty()) {
      emplace(blocks.end(), std::move(b));
      return;
    }

    // Find the right place (in ascending ptr order) to insert the block
    // Can't use binary_search because it's a linked list and will be quadratic
    auto const next =
      std::find_if(blocks.begin(), blocks.end(), [&b](block_t const& i) { return b < i; });
    auto previous = (next == blocks.begin()) ? next : std::prev(next);

    // Coalesce with neighboring blocks or insert the new block if it can't be coalesced
    bool const merge_prev = previous->is_contiguous_before(b);
    bool const merge_next = (next != blocks.end()) && b.is_contiguous_before(*next);

    if (merge_prev && merge_next) {
      //*previous = previous->merge(b).merge(*next);
      previous->merge(std::move(b));
      previous->merge(std::move(*next));
      erase(next);
    } else if (merge_prev) {
      previous->merge(std::move(b));
    } else if (merge_next) {
      *next = b.merge(std::move(*next));
    } else {
      emplace(next, std::move(b));  // cannot be coalesced, just insert
    }
  }

  /**
   * @brief Inserts blocks from range `[first, last)` into the free_list in their correct order,
   *        coalescing them with their preceding and following blocks if they are contiguous.
   *
   * @tparam InputIt iterator type
   * @param first The beginning of the range of blocks to insert
   * @param last The end of the range of blocks to insert.
   */
  template <class InputIt>
  void insert(InputIt first, InputIt last)
  {
    std::for_each(first, last, [this](block_t const& b) { this->insert(b); });
  }

  /**
   * @brief Removes the block indicated by `iter` from the free list.
   *
   * @param iter An iterator referring to the block to erase.
   */
  void erase(const_iterator iter) { blocks.erase(iter); }

  /**
   * @brief Erase all blocks from the free_list.
   *
   */
  void clear() noexcept { blocks.clear(); }

  /**
   * @brief Finds the smallest block in the `free_list` large enough to fit `size` bytes.
   *
   * @param size The size in bytes of the desired block.
   * @return block A block large enough to store `size` bytes.
   */
  block_t best_fit(size_t size)
  {
    // find best fit block
    auto iter = std::min_element(
      blocks.begin(), blocks.end(), [size](block_t const& lhs, block_t const& rhs) {
        return lhs.is_better_fit(size, rhs);
      });

    if (iter != blocks.end() && iter->fits(size)) {
      // Remove the block from the free_list and return it.
      block_t const found{std::move(*iter)};
      erase(iter);
      return found;
    }

    return block_t{};  // not found
  }

  /**
   * @brief Print all blocks in the free_list.
   */
  void print() const
  {
    std::cout << blocks.size() << '\n';
    for (block_t const& b : blocks) {
      b.print();
    }
  }

 protected:
  /**
   * @brief Insert a block in the free list before the specified position
   *
   * @param pos iterator before which the block will be inserted. pos may be the end() iterator.
   * @param b The block to insert.
   */
  void insert(const_iterator pos, block_t const& b) { blocks.insert(pos, b); }

  template <typename... Args>
  void emplace(const_iterator pos, Args&&... args)
  {
    blocks.emplace(pos, std::forward<Args>(args)...);
  }

 private:
  list_t blocks;  // The internal container of blocks
};

}  // namespace detail
}  // namespace mr
}  // namespace rmm
