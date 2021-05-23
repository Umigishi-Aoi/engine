// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <cstddef>
#include <functional>
#include <map>
#include <string>
#include <type_traits>

#include "flutter/fml/hash_combine.h"
#include "flutter/fml/macros.h"

namespace impeller {

class ComparableBase {};

template <class Type>
class Comparable : ComparableBase {
 public:
  virtual std::size_t GetHash() const = 0;

  virtual bool IsEqual(const Type& other) const = 0;
};

template <
    class ComparableType,
    class = std::enable_if_t<std::is_base_of_v<ComparableBase, ComparableType>>>
struct ComparableHash {
  std::size_t operator()(const ComparableType& object) const {
    return object.GetHash();
  }
};

template <
    class ComparableType,
    class = std::enable_if_t<std::is_base_of_v<ComparableBase, ComparableType>>>
struct ComparableEqual {
  bool operator()(const ComparableType& lhs, const ComparableType& rhs) const {
    return lhs.IsEqual(rhs);
  }
};

template <
    class ComparableType,
    class = std::enable_if_t<std::is_base_of_v<ComparableBase, ComparableType>>>
bool DeepComparePointer(const std::shared_ptr<ComparableType>& lhs,
                        const std::shared_ptr<ComparableType>& rhs) {
  if (lhs == rhs) {
    return true;
  }

  if (lhs && rhs) {
    return lhs->IsEqual(*rhs);
  }

  return false;
}

template <
    class Key,
    class ComparableType,
    class = std::enable_if_t<std::is_base_of_v<ComparableBase, ComparableType>>>
bool DeepCompareMap(const std::map<Key, std::shared_ptr<ComparableType>>& lhs,
                    const std::map<Key, std::shared_ptr<ComparableType>>& rhs) {
  if (lhs.size() != rhs.size()) {
    return false;
  }

  for (auto i = lhs.begin(), j = rhs.begin(); i != lhs.end(); i++, j++) {
    if (i->first != j->first) {
      return false;
    }

    if (!DeepComparePointer(i->second, j->second)) {
      return false;
    }
  }

  return true;
}

}  // namespace impeller
