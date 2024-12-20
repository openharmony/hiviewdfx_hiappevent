/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#ifndef HIAPPEVENT_FRAMEWORKS_NATIVE_LIB_HIAPPEVENT_CACHE_APP_EVENT_OBSERVER_DAO_H
#define HIAPPEVENT_FRAMEWORKS_NATIVE_LIB_HIAPPEVENT_CACHE_APP_EVENT_OBSERVER_DAO_H

#include <memory>
#include <string>
#include <vector>

#include "hiappevent_base.h"
#include "rdb_store.h"

namespace OHOS {
namespace HiviewDFX {

class AppEventObserverDao {
public:
    AppEventObserverDao(std::shared_ptr<NativeRdb::RdbStore> dbStore);
    ~AppEventObserverDao() = default;
    int64_t Insert(const std::string& observer, int64_t hashCode);
    int64_t QuerySeq(const std::string& observer, int64_t hashCode);
    int QuerySeqs(const std::string& observer, std::vector<int64_t>& observerSeqs, ObserverType type);
    int Delete(const std::string& observer);
    int Delete(int64_t observerSeq, ObserverType type);

private:
    int Create();

private:
    std::shared_ptr<NativeRdb::RdbStore> dbStore_;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIAPPEVENT_FRAMEWORKS_NATIVE_LIB_HIAPPEVENT_CACHE_APP_EVENT_OBSERVER_DAO_H
