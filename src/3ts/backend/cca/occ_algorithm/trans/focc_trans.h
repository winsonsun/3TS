/*
 * Tencent is pleased to support the open source community by making 3TS available.
 *
 * Copyright (C) 2020 THL A29 Limited, a Tencent company.  All rights reserved. The below software
 * in this distribution may have been modified by THL A29 Limited ("Tencent Modifications"). All
 * Tencent Modifications are Copyright (C) THL A29 Limited.
 *
 * Author: elioyan@tencent.com
 *
 */
#pragma once
#include "../env/si_env.h"

namespace ttts {
namespace occ_algorithm {
class FoccTransactionDesc
    : public SITransactionDesc<FoccTransactionDesc, SIEnvironmentDesc, Anomally> {
 public:
  static std::string name;

  FoccTransactionDesc(const uint64_t trans_id, const uint64_t start_ts, Snapshot&& snapshot,
                      env_desc_type& env_desc)
      : SITransactionDesc(trans_id, start_ts, std::move(snapshot), env_desc) {}
  virtual std::optional<Anomally> CheckConflict(const uint64_t commit_ts) override {
    // History history_with_write_version = env_desc_.GetHistory();
    for (const auto& ptr : env_desc_.active_trans_) {
      if (ptr.second->start_ts() < this->commit_ts() && ptr.second->commit_ts() == 0) {
        ptr.second->set_back_check();
        if (Intersect(w_items_, ptr.second->r_items_)) {
          return Anomally::UNKNOWN;
        }
      }
    }
    for (const auto& ptr : env_desc_.commit_trans_) {
      // if (ptr.second->commit_ts() > start_ts() && ptr.second->commit_ts() < this->commit_ts()) {
      // once it
      if (back_check()) {
        if (Intersect(r_items_, ptr.second->w_items_)) {
          return Anomally::UNKNOWN;
        }
      }
    }
    return {};
  }

 private:
  bool Intersect(set_type& left, set_type& right) {
    for (auto& a : left) {
      if (right.count(a.first) > 0) return true;
    }
    return false;
  }
};
std::string FoccTransactionDesc::name = "FoccTransactionDesc";

}  // namespace occ_algorithm
}  // namespace ttts
