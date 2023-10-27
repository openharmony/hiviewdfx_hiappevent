/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#ifndef HIAPPEVENT_FRAMEWORKS_NATIVE_LIB_HIAPPEVENT_OBSERVER_APP_EVENT_OBSERVER_MGR_H
#define HIAPPEVENT_FRAMEWORKS_NATIVE_LIB_HIAPPEVENT_OBSERVER_APP_EVENT_OBSERVER_MGR_H

#include <memory>
#include <shared_mutex>
#include <unordered_map>

#include "app_event_observer.h"
#include "singleton.h"

namespace OHOS {
namespace HiviewDFX {
class AppEventHandler;
using HiAppEvent::AppEventObserver;
using HiAppEvent::ReportConfig;

class AppEventObserverMgr : public DelayedRefSingleton<AppEventObserverMgr> {
public:
    AppEventObserverMgr();
    ~AppEventObserverMgr();

    int64_t RegisterObserver(std::shared_ptr<AppEventObserver> observer);
    int64_t RegisterObserver(const std::string& observerName);
    int UnregisterObserver(int64_t observerSeq);
    int UnregisterObserver(const std::string& observerName);
    void HandleEvent(std::shared_ptr<AppEventPack> event);
    void HandleTimeOut();
    void HandleStartup();
    void HandleBackground();

    int SetReportConfig(int64_t observerSeq, const ReportConfig& config);
    int GetReportConfig(int64_t observerSeq, ReportConfig& config);

private:
    bool CreateEventHandler();
    void DestroyEventHandler();

private:
    std::unordered_map<int64_t, std::shared_ptr<AppEventObserver>> observers_;
    std::unordered_map<int64_t, ReportConfig> configs_;
    std::shared_ptr<AppEventHandler> handler_;
    std::shared_mutex mutex_;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIAPPEVENT_FRAMEWORKS_NATIVE_LIB_HIAPPEVENT_OBSERVER_APP_EVENT_OBSERVER_MGR_H