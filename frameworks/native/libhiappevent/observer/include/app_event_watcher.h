/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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
#ifndef HIAPPEVENT_FRAMEWORKS_NATIVE_LIB_HIAPPEVENT_OBSERVER_APP_EVENT_WATCHER_H
#define HIAPPEVENT_FRAMEWORKS_NATIVE_LIB_HIAPPEVENT_OBSERVER_APP_EVENT_WATCHER_H

#include "app_event_observer.h"

namespace OHOS {
namespace HiviewDFX {
using HiAppEvent::AppEventObserver;
using HiAppEvent::TriggerCondition;
using HiAppEvent::AppEventFilter;

class AppEventWatcher : public AppEventObserver {
public:
    explicit AppEventWatcher(const std::string& name);

    AppEventWatcher(
        const std::string& name,
        const std::vector<AppEventFilter>& filters,
        TriggerCondition cond);
    virtual ~AppEventWatcher() {}
    // used to match os events.
    uint64_t GetOsEventsMask();
    void SetFiltersStr(const std::string& jsonStr);
    std::string GetFiltersStr();

private:
    std::string filtersStr_;
    std::mutex mutex_;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIAPPEVENT_FRAMEWORKS_NATIVE_LIB_HIAPPEVENT_OBSERVER_APP_EVENT_WATCHER_H
