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
#include "napi_hiappevent_init.h"

#include <map>
#include <string>

#include "napi_util.h"

namespace OHOS {
namespace HiviewDFX {
namespace NapiHiAppEventInit {
namespace {
constexpr int FAULT_EVENT_TYPE = 1;
constexpr int STATISTIC_EVENT_TYPE = 2;
constexpr int SECURITY_EVENT_TYPE = 3;
constexpr int BEHAVIOR_EVENT_TYPE = 4;

const std::string EVENT_CLASS_NAME = "Event";
const std::string PARAM_CLASS_NAME = "Param";
const std::string EVENT_TYPE_CLASS_NAME = "EventType";

napi_value ClassConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value argv = nullptr;
    napi_value thisArg = nullptr;
    void* data = nullptr;
    napi_get_cb_info(env, info, &argc, &argv, &thisArg, &data);

    napi_value global = 0;
    napi_get_global(env, &global);

    return thisArg;
}

void InitEventTypeMap(napi_env env, std::map<const char*, napi_value>& eventTypeMap)
{
    eventTypeMap["FAULT"] = NapiUtil::CreateInt32(env, FAULT_EVENT_TYPE);
    eventTypeMap["STATISTIC"] = NapiUtil::CreateInt32(env, STATISTIC_EVENT_TYPE);
    eventTypeMap["SECURITY"] = NapiUtil::CreateInt32(env, SECURITY_EVENT_TYPE);
    eventTypeMap["BEHAVIOR"] = NapiUtil::CreateInt32(env, BEHAVIOR_EVENT_TYPE);
}

void InitEventMap(napi_env env, std::map<const char*, napi_value>& eventMap)
{
    eventMap["USER_LOGIN"] = NapiUtil::CreateString(env, "hiappevent.user_login");
    eventMap["USER_LOGOUT"] = NapiUtil::CreateString(env, "hiappevent.user_logout");
    eventMap["DISTRIBUTED_SERVICE_START"] = NapiUtil::CreateString(env, "hiappevent.distributed_service_start");
}

void InitParamMap(napi_env env, std::map<const char*, napi_value>& paramMap)
{
    paramMap["USER_ID"] = NapiUtil::CreateString(env, "user_id");
    paramMap["DISTRIBUTED_SERVICE_NAME"] = NapiUtil::CreateString(env, "ds_name");
    paramMap["DISTRIBUTED_SERVICE_INSTANCE_ID"] = NapiUtil::CreateString(env, "ds_instance_id");
}

void InitConstClassByName(napi_env env, napi_value exports, const std::string& name)
{
    std::map<const char*, napi_value> propertyMap;
    if (name == EVENT_CLASS_NAME) {
        InitEventMap(env, propertyMap);
    } else if (name == PARAM_CLASS_NAME) {
        InitParamMap(env, propertyMap);
    } else if (name == EVENT_TYPE_CLASS_NAME) {
        InitEventTypeMap(env, propertyMap);
    } else {
        return;
    }

    int i = 0;
    napi_property_descriptor descriptors[propertyMap.size()];
    for (auto it : propertyMap) {
        descriptors[i++] = DECLARE_NAPI_STATIC_PROPERTY(it.first, it.second);
    }

    napi_value result = nullptr;
    napi_define_class(env, name.c_str(), NAPI_AUTO_LENGTH, ClassConstructor, nullptr,
        sizeof(descriptors) / sizeof(*descriptors), descriptors, &result);
    napi_set_named_property(env, exports, name.c_str(), result);
}
}

napi_value InitNapiClass(napi_env env, napi_value exports)
{
    InitConstClassByName(env, exports, EVENT_CLASS_NAME);
    InitConstClassByName(env, exports, PARAM_CLASS_NAME);
    InitConstClassByName(env, exports, EVENT_TYPE_CLASS_NAME);
    return exports;
}
} // namespace NapiHiAppEventInit
} // namespace HiviewDFX
} // namespace OHOS
