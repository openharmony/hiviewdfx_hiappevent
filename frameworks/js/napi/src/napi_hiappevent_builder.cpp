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
#include "napi_hiappevent_builder.h"

#include "hiappevent_base.h"
#include "hiappevent_pack.h"
#include "hilog/log.h"
#include "napi_util.h"

namespace OHOS {
namespace HiviewDFX {
namespace {
const HiLogLabel LABEL = { LOG_CORE, HIAPPEVENT_DOMAIN, "NapiHiAppEventBuilder" };
const std::string DOMAIN_PROPERTY = "domain";
const std::string NAME_PROPERTY = "name";
const std::string TYPE_PROPERTY = "eventType";
const std::string PARAM_PROPERTY = "params";
constexpr size_t MAX_STRING_LEN = 8 * 1024 + 2; // 2 for '\0' and extra symbol
}
using namespace OHOS::HiviewDFX::ErrorCode;

bool NapiHiAppEventBuilder::IsValidEventDomain(const napi_env env, const napi_value domain)
{
    if (!NapiUtil::IsString(env, domain)) {
        HiLog::Error(LABEL, "event domain must be of type string");
        return false;
    }
    return true;
}

bool NapiHiAppEventBuilder::IsValidEventName(const napi_env env, const napi_value name)
{
    if (!NapiUtil::IsString(env, name)) {
        HiLog::Error(LABEL, "event name must be of type string");
        return false;
    }
    return true;
}

bool NapiHiAppEventBuilder::IsValidEventType(const napi_env env, const napi_value type)
{
    if (!NapiUtil::IsNumber(env, type)) {
        HiLog::Error(LABEL, "event type must be of type number");
        return false;
    }
    return true;
}

bool NapiHiAppEventBuilder::IsValidEventParam(const napi_env env, const napi_value param)
{
    if (!NapiUtil::IsObject(env, param)) {
        HiLog::Error(LABEL, "event param must be of type object");
        return false;
    }
    return true;
}

bool NapiHiAppEventBuilder::IsValidEventInfo(const napi_env env, const napi_value eventInfo)
{
    if (!NapiUtil::IsObject(env, eventInfo)) {
        HiLog::Error(LABEL, "eventInfo must be of type object");
        return false;
    }
    return IsValidEventDomain(env, NapiUtil::GetProperty(env, eventInfo, DOMAIN_PROPERTY))
        && IsValidEventName(env, NapiUtil::GetProperty(env, eventInfo, NAME_PROPERTY))
        && IsValidEventType(env, NapiUtil::GetProperty(env, eventInfo, TYPE_PROPERTY))
        && IsValidEventParam(env, NapiUtil::GetProperty(env, eventInfo, PARAM_PROPERTY));
}

bool NapiHiAppEventBuilder::IsOldWriteParams(const napi_env env, const napi_value params[], size_t len)
{
    if (len != 3 && len != 4) { // The number of params that the old write interface can receive is 3 or 4
        return false;
    }
    return IsValidEventName(env, params[0]) // 0 means the index of event name
        && IsValidEventType(env, params[1]) // 1 means the index of event type
        && IsValidEventParam(env, params[2]); // 2 means the index of event param
}

bool NapiHiAppEventBuilder::IsNewWriteParams(const napi_env env, const napi_value params[], size_t len)
{
    if (len != 1 && len != 2) { // The number of params that the new write interface can receive is 1 or 2
        return false;
    }
    return IsValidEventInfo(env, params[0]);
}

void NapiHiAppEventBuilder::AddArrayParam2EventPack(napi_env env, const std::string &key, const napi_value arr)
{
    napi_valuetype type = NapiUtil::GetArrayType(env, arr);
    switch (type) {
        case napi_boolean: {
            std::vector<bool> bools;
            NapiUtil::GetBooleans(env, arr, bools);
            AddEventParam(appEventPack_, key, bools);
            break;
        }
        case napi_number: {
            std::vector<double> doubles;
            NapiUtil::GetDoubles(env, arr, doubles);
            AddEventParam(appEventPack_, key, doubles);
            break;
        }
        case napi_string: {
            std::vector<std::string> strs;
            NapiUtil::GetStrings(env, arr, strs, MAX_STRING_LEN);
            AddEventParam(appEventPack_, key, strs);
            break;
        }
        case napi_null: {
            AddEventParam(appEventPack_, key);
            break;
        }
        default: {
            HiLog::Error(LABEL, "array param value type is invalid");
            result_ = ERROR_INVALID_LIST_PARAM_TYPE;
            break;
        }
    }
}

void NapiHiAppEventBuilder::AddParam2EventPack(napi_env env, const std::string &key, const napi_value value)
{
    napi_valuetype type = NapiUtil::GetType(env, value);
    switch (type) {
        case napi_boolean:
            AddEventParam(appEventPack_, key, NapiUtil::GetBoolean(env, value));
            break;
        case napi_number:
            AddEventParam(appEventPack_, key, NapiUtil::GetDouble(env, value));
            break;
        case napi_string:
            AddEventParam(appEventPack_, key, NapiUtil::GetString(env, value));
            break;
        case napi_object:
            if (NapiUtil::IsArray(env, value)) {
                AddArrayParam2EventPack(env, key, value);
                break;
            }
        default:
            HiLog::Error(LABEL, "param value type is invalid");
            result_ = ERROR_INVALID_PARAM_VALUE_TYPE;
            break;
    }
}

void NapiHiAppEventBuilder::AddParams2EventPack(napi_env env, const napi_value paramObj)
{
    std::vector<std::string> keys;
    NapiUtil::GetPropertyNames(env, paramObj, keys);
    for (auto key : keys) {
        napi_value value = NapiUtil::GetProperty(env, paramObj, key);
        if (value == nullptr) {
            result_ = ERROR_INVALID_PARAM_VALUE_TYPE;
            continue;
        }
        AddParam2EventPack(env, key, value);
    }
}

void NapiHiAppEventBuilder::BuildEventPack(napi_env env, const napi_value params[])
{
    int index = 0;
    std::string name = NapiUtil::GetString(env, params[index++]);
    int32_t type = NapiUtil::GetInt32(env, params[index++]);
    appEventPack_ = std::make_shared<AppEventPack>(name, type);
    AddParams2EventPack(env, params[index]);
}

void NapiHiAppEventBuilder::BuildEventPack(napi_env env, const napi_value eventInfo)
{
    std::string domain = NapiUtil::GetString(env, NapiUtil::GetProperty(env, eventInfo, DOMAIN_PROPERTY));
    std::string name = NapiUtil::GetString(env, NapiUtil::GetProperty(env, eventInfo, NAME_PROPERTY));
    int32_t type = NapiUtil::GetInt32(env, NapiUtil::GetProperty(env, eventInfo, TYPE_PROPERTY));
    appEventPack_ = std::make_shared<AppEventPack>(domain, name, type);
    napi_value param = NapiUtil::GetProperty(env, eventInfo, PARAM_PROPERTY);
    AddParams2EventPack(env, param);
}

void NapiHiAppEventBuilder::BuildCallback(const napi_env env, const napi_value callback)
{
    if (NapiUtil::IsFunction(env, callback)) {
        callback_ = NapiUtil::CreateReference(env, callback);
    }
}

std::shared_ptr<AppEventPack> NapiHiAppEventBuilder::Build(const napi_env env, const napi_value params[], size_t len)
{
    if (len < 1 || len > 4) { // The min number of params is 1, and the max number of params is 4
        result_ = ERROR_INVALID_PARAM_NUM_JS;
        return nullptr;
    }

    // 1. build AppEventPack
    if (IsOldWriteParams(env, params, len)) {
        BuildEventPack(env, params);
    } else if (IsNewWriteParams(env, params, len)) {
        BuildEventPack(env, params[0]);
    } else {
        result_ = ERROR_INVALID_PARAM_TYPE_JS;
    }

    // 2. build callback if any
    BuildCallback(env, params[len - 1]); // (len - 1) means the last param
    return appEventPack_;
}

int NapiHiAppEventBuilder::GetResult() const
{
    return result_;
}

napi_ref NapiHiAppEventBuilder::GetCallback() const
{
    return callback_;
}
} // namespace HiviewDFX
} // namespace OHOS