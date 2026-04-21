/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef HIAPPEVENT_FRAMEWORKS_NATIVE_LIB_HIAPPEVENT_CACHE_API_STATS_DAO_H
#define HIAPPEVENT_FRAMEWORKS_NATIVE_LIB_HIAPPEVENT_CACHE_API_STATS_DAO_H

#include <memory>
#include <string>
#include <vector>

#include "rdb_store.h"

namespace OHOS {
namespace HiviewDFX {
namespace ApiStatsDao {

int Create(NativeRdb::RdbStore& dbStore);
int MetricInsert(std::shared_ptr<NativeRdb::RdbStore> dbStore, const std::string& kitName, const std::string& apiName,
    const std::string& metric);
int MetricQueryAll(std::shared_ptr<NativeRdb::RdbStore> dbStore,
    std::map<std::pair<std::string, std::string>, std::vector<std::string>>& out);
int MetricClear(std::shared_ptr<NativeRdb::RdbStore> dbStore);
} // namespace ApiStatsDao
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIAPPEVENT_FRAMEWORKS_NATIVE_LIB_HIAPPEVENT_CACHE_API_STATS_DAO_H
