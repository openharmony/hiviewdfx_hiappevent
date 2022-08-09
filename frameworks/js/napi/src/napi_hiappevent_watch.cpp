/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "napi_hiappevent_watch.h"

#include <string>

#include "app_event_cache.h"
#include "app_event_watcher_mgr.h"
#include "hiappevent_base.h"
#include "hiappevent_config.h"
#include "hiappevent_verify.h"
#include "hilog/log.h"
#include "napi_app_event_holder.h"
#include "napi_app_event_watcher.h"
#include "napi_util.h"

namespace OHOS {
namespace HiviewDFX {
namespace NapiHiAppEventWatch {
namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, HIAPPEVENT_DOMAIN, "Napi_HiAppEvent_Watch" };
const std::string NAME_PROPERTY = "name";
const std::string COND_PROPERTY = "triggerCondition";
const std::string COND_PROPS[] = { "row", "size", "timeOut" };
const std::string FILTERS_PROPERTY = "appEventFilters";
const std::string FILTERS_DOAMIN_PROP = "domain";
const std::string FILTERS_TYPES_PROP = "eventTypes";
const std::string TRIGGER_PROPERTY = "onTrigger";
constexpr int BIT_MASK = 1;
constexpr unsigned int BIT_ALL_TYPES = 0xff;

bool IsValidName(const napi_env env, const napi_value name)
{
    if (!NapiUtil::IsString(env, name)) {
        return false;
    }
    return IsValidWatcherName(NapiUtil::GetString(env, name));
}

bool IsValidConditon(const napi_env env, const napi_value cond)
{
    if (cond == nullptr) {
        return true;
    }
    if (!NapiUtil::IsObject(env, cond)) {
        return false;
    }
    for (auto& propName : COND_PROPS) {
        napi_value propValue = NapiUtil::GetProperty(env, cond, propName);
        if (propValue == nullptr) {
            continue;
        }
        if (!NapiUtil::IsNumber(env, propValue)) {
            return false;
        }
    }
    return true;
}

bool IsValidFilter(const napi_env env, const napi_value filter)
{
    napi_value domain = NapiUtil::GetProperty(env, filter, FILTERS_DOAMIN_PROP);
    if (domain == nullptr || !NapiUtil::IsString(env, domain) || !IsValidDomain(NapiUtil::GetString(env, domain))) {
        return false;
    }
    napi_value types = NapiUtil::GetProperty(env, filter, FILTERS_TYPES_PROP);
    if (types == nullptr) {
        return true;
    }
    return NapiUtil::IsArrayType(env, types, napi_number);
}


bool IsValidFilters(const napi_env env, const napi_value filters)
{
    if (filters == nullptr) {
        return true;
    }
    if (!NapiUtil::IsArrayType(env, filters, napi_object)) {
        return false;
    }

    size_t len = NapiUtil::GetArrayLength(env, filters);
    for (size_t i = 0; i < len; i++) {
        napi_value filter = NapiUtil::GetElement(env, filters, i);
        if (!IsValidFilter(env, filter)) {
            return false;
        }
    }
    return true;
}

bool IsValidTrigger(const napi_env env, const napi_value trigger)
{
    if (trigger == nullptr) {
        return true;
    }
    return NapiUtil::IsFunction(env, trigger);
}

bool IsValidWatcher(const napi_env env, const napi_value watcher)
{
    if (!NapiUtil::IsObject(env, watcher)) {
        return false;
    }
    return IsValidName(env, NapiUtil::GetProperty(env, watcher, NAME_PROPERTY))
        && IsValidConditon(env, NapiUtil::GetProperty(env, watcher, COND_PROPERTY))
        && IsValidFilters(env, NapiUtil::GetProperty(env, watcher, FILTERS_PROPERTY))
        && IsValidTrigger(env, NapiUtil::GetProperty(env, watcher, TRIGGER_PROPERTY));
}

std::string GetName(const napi_env env, const napi_value watcher)
{
    return NapiUtil::GetString(env, NapiUtil::GetProperty(env, watcher, NAME_PROPERTY));
}

TriggerCondition GetCondition(const napi_env env, const napi_value watcher)
{
    TriggerCondition resCond = {
        .row = 0,
        .size = 0,
        .timeOut = 0
    };
    napi_value cond = NapiUtil::GetProperty(env, watcher, COND_PROPERTY);
    if (cond == nullptr) {
        return resCond;
    }
    size_t index = 0;
    napi_value rowValue = NapiUtil::GetProperty(env, cond, COND_PROPS[index++]);
    resCond.row = rowValue != nullptr ? NapiUtil::GetInt32(env, rowValue) : 0;
    napi_value sizeValue = NapiUtil::GetProperty(env, cond, COND_PROPS[index++]);
    resCond.size = sizeValue != nullptr ? NapiUtil::GetInt32(env, sizeValue) : 0;
    napi_value timeOutValue = NapiUtil::GetProperty(env, cond, COND_PROPS[index]);
    resCond.timeOut = timeOutValue != nullptr ? NapiUtil::GetInt32(env, timeOutValue) : 0;
    return resCond;
}

void GetFilters(const napi_env env, const napi_value watcher, std::map<std::string, unsigned int>& filters)
{
    napi_value filtersValue = NapiUtil::GetProperty(env, watcher, FILTERS_PROPERTY);
    if (filtersValue == nullptr) {
        return;
    }
    size_t len = NapiUtil::GetArrayLength(env, filtersValue);
    for (size_t i = 0; i < len; i++) {
        napi_value filterValue = NapiUtil::GetElement(env, filtersValue, i);
        std::string domain = NapiUtil::GetString(env, NapiUtil::GetProperty(env, filterValue, FILTERS_DOAMIN_PROP));
        napi_value typesValue = NapiUtil::GetProperty(env, filterValue, FILTERS_TYPES_PROP);
        if (typesValue == nullptr) {
            filters[domain] = BIT_ALL_TYPES;
            continue;
        }
        std::vector<int> types;
        NapiUtil::GetInt32s(env, typesValue, types);
        unsigned int filterType = 0;
        for (auto type : types) {
            if (type < 1 || type > 4) { // 1-4: value range of event type
                continue;
            }
            filterType |= (BIT_MASK << type);
        }
        filters[domain] = filterType > 0 ? filterType : BIT_ALL_TYPES;
    }
}

napi_value CreateHolder(const napi_env env, const napi_value watcherName)
{
    napi_value constructor = nullptr;
    if (napi_get_reference_value(env, NapiAppEventHolder::constructor_, &constructor) != napi_ok) {
        HiLog::Error(LABEL, "failed to get constructor of the holder");
        return NapiUtil::CreateNull(env);
    }
    napi_value holder = nullptr;
    if (napi_new_instance(env, constructor, 1, &watcherName, &holder) != napi_ok) { // param num is 1
        HiLog::Error(LABEL, "failed to get new instance for holder");
        return NapiUtil::CreateNull(env);
    }
    return holder;
}
}

napi_value AddWatcher(const napi_env env, const napi_value watcher)
{
    if (!IsValidWatcher(env, watcher)) {
        HiLog::Error(LABEL, "invalid watcher");
        return NapiUtil::CreateNull(env);
    }

    // 1. if DB is not opened, need to open DB first
    if (!AppEventCache::GetInstance()->IsOpen()) {
        std::string dir = HiAppEventConfig::GetInstance().GetStorageDir();
        if (dir.empty() || AppEventCache::GetInstance()->Open(dir) != 0) {
            HiLog::Error(LABEL, "failed to open db");
            return NapiUtil::CreateNull(env);
        }
    }

    // 2. build watcher object
    std::map<std::string, unsigned int> filters;
    GetFilters(env, watcher, filters);
    std::string name = GetName(env, watcher);
    TriggerCondition cond = GetCondition(env, watcher);
    auto watcherPtr = std::make_shared<NapiAppEventWatcher>(name, filters, cond);

    // 3. create holder and add holder to the watcher
    napi_value holder = CreateHolder(env, NapiUtil::GetProperty(env, watcher, NAME_PROPERTY));
    watcherPtr->InitHolder(env, holder);

    // 4. set trigger if any
    napi_value trigger = NapiUtil::GetProperty(env, watcher, TRIGGER_PROPERTY);
    if (trigger != nullptr) {
        watcherPtr->InitTrigger(env, trigger);
    }

    // 5. add the watcher to watcherManager
    AppEventWatcherMgr::GetInstance()->AddWatcher(watcherPtr);
    return holder;
}

napi_value RemoveWatcher(const napi_env env, const napi_value watcher)
{
    if (!NapiUtil::IsObject(env, watcher) || !IsValidName(env, NapiUtil::GetProperty(env, watcher, NAME_PROPERTY))) {
        return NapiUtil::CreateUndefined(env);
    }
    AppEventWatcherMgr::GetInstance()->RemoveWatcher(GetName(env, watcher));
    return NapiUtil::CreateUndefined(env);
}
} // namespace NapiHiAppEventConfig
} // namespace HiviewDFX
} // namespace OHOS
