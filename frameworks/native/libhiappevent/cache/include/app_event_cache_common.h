/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#ifndef HIAPPEVENT_FRAMEWORKS_NATIVE_LIB_HIAPPEVENT_CACHE_APP_EVENT_CACHE_COMMON_H
#define HIAPPEVENT_FRAMEWORKS_NATIVE_LIB_HIAPPEVENT_CACHE_APP_EVENT_CACHE_COMMON_H

#include <string>

namespace OHOS {
namespace HiviewDFX {
namespace AppEventCacheCommon {
constexpr int DB_SUCC = 0;
constexpr int DB_FAILED = -1;
const unsigned int HIAPPEVENT_DOMAIN = 0xD002D07;

namespace Events {
const std::string TABLE = "events";
const std::string FIELD_SEQ = "seq";
const std::string FIELD_DOMAIN = "domain";
const std::string FIELD_NAME = "name";
const std::string FIELD_TYPE = "type";
const std::string FIELD_TIME = "time";
const std::string FIELD_TZ = "tz";
const std::string FIELD_PID = "pid";
const std::string FIELD_TID = "tid";
const std::string FIELD_TRACE_ID = "trace_id";
const std::string FIELD_SPAN_ID = "span_id";
const std::string FIELD_PSPAN_ID = "pspan_id";
const std::string FIELD_TRACE_FLAG = "trace_flag";
const std::string FIELD_PARAMS = "params";
const std::string FIELD_SIZE = "size";
const std::string FIELD_RUNNING_ID = "running_id";
} // namespace Events

namespace Observers {
const std::string TABLE = "observers";
const std::string FIELD_SEQ = "seq";
const std::string FIELD_NAME = "name";
const std::string FIELD_HASH = "hash";
const std::string FIELD_FILTERS = "filters";
} // namespace Observers

struct Observer {
    Observer(int64_t seq, const std::string& name, const std::string& filters)
        : seq(seq), name(name), filters(filters) {}
    ~Observer() {}
    int64_t seq = 0;
    std::string name;
    std::string filters;
};

namespace AppEventMapping {
const std::string TABLE = "event_observer_mapping";
const std::string FIELD_SEQ = "seq";
const std::string FIELD_EVENT_SEQ = "event_seq";
const std::string FIELD_OBSERVER_SEQ = "observer_seq";
} // namespace AppEventMapping

namespace UserIds {
const std::string TABLE = "user_ids";
const std::string FIELD_SEQ = "seq";
const std::string FIELD_NAME = "name";
const std::string FIELD_VALUE = "value";
} // namespace UserIds

namespace UserProperties {
const std::string TABLE = "user_properties";
const std::string FIELD_SEQ = "seq";
const std::string FIELD_NAME = "name";
const std::string FIELD_VALUE = "value";
} // namespace UserProperties

namespace CustomEventParams {
const std::string TABLE = "custom_event_params";
const std::string FIELD_SEQ = "seq";
const std::string FIELD_RUNNING_ID = "running_id";
const std::string FIELD_DOMAIN = "domain";
const std::string FIELD_NAME = "name";
const std::string FIELD_PARAM_KEY = "param_key";
const std::string FIELD_PARAM_VALUE = "param_value";
const std::string FIELD_PARAM_TYPE = "param_type";
} // namespace CustomEventParams
} // namespace AppEventCacheCommon
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIAPPEVENT_FRAMEWORKS_NATIVE_LIB_HIAPPEVENT_CACHE_APP_EVENT_CACHE_COMMON_H
