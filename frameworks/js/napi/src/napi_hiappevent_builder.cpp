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
#include "hiappevent_verify.h"
#include "hilog/log.h"
#include "napi_error.h"
#include "napi_util.h"

namespace OHOS {
namespace HiviewDFX {
namespace {
const HiLogLabel LABEL = { LOG_CORE, HIAPPEVENT_DOMAIN, "Napi_HiAppEvent_Builder" };
const std::string DOMAIN_PROPERTY = "domain";
const std::string NAME_PROPERTY = "name";
const std::string TYPE_PROPERTY = "eventType";
const std::string PARAM_PROPERTY = "params";
constexpr size_t MAX_LENGTH_OF_PARAM_NAME = 16;
const std::string PARAM_VALUE_TYPE = "boolean|number|string|array[boolean|number|string]";
}
using namespace OHOS::HiviewDFX::ErrorCode;

bool NapiHiAppEventBuilder::IsValidEventDomain(const napi_env env, const napi_value domain)
{
    if (domain == nullptr) {
        NapiUtil::ThrowError(env, NapiError::ERR_PARAM, NapiUtil::CreateErrMsg("domain"), isV9_);
        return false;
    }
    if (!NapiUtil::IsString(env, domain)) {
        NapiUtil::ThrowError(env, NapiError::ERR_PARAM, NapiUtil::CreateErrMsg("domain", "string"), isV9_);
        return false;
    }
    return true;
}

bool NapiHiAppEventBuilder::IsValidEventName(const napi_env env, const napi_value name)
{
    if (name == nullptr) {
        NapiUtil::ThrowError(env, NapiError::ERR_PARAM, NapiUtil::CreateErrMsg("name"), isV9_);
        return false;
    }
    if (!NapiUtil::IsString(env, name)) {
        NapiUtil::ThrowError(env, NapiError::ERR_PARAM, NapiUtil::CreateErrMsg("name", "string"), isV9_);
        return false;
    }
    return true;
}

bool NapiHiAppEventBuilder::IsValidEventType(const napi_env env, const napi_value type)
{
    if (type == nullptr) {
        NapiUtil::ThrowError(env, NapiError::ERR_PARAM, NapiUtil::CreateErrMsg("eventType"), isV9_);
        return false;
    }
    if (!NapiUtil::IsNumber(env, type) || !OHOS::HiviewDFX::IsValidEventType(NapiUtil::GetInt32(env, type))) {
        NapiUtil::ThrowError(env, NapiError::ERR_PARAM, NapiUtil::CreateErrMsg("eventType", "EventType"), isV9_);
        return false;
    }
    return true;
}

bool NapiHiAppEventBuilder::IsValidEventParam(const napi_env env, const napi_value param)
{
    if (param == nullptr) {
        NapiUtil::ThrowError(env, NapiError::ERR_PARAM, NapiUtil::CreateErrMsg("params"), isV9_);
        return false;
    }
    if (!NapiUtil::IsObject(env, param)) {
        NapiUtil::ThrowError(env, NapiError::ERR_PARAM, NapiUtil::CreateErrMsg("params", "object"), isV9_);
        return false;
    }
    return true;
}

bool NapiHiAppEventBuilder::IsValidEventInfo(const napi_env env, const napi_value eventInfo)
{
    if (!NapiUtil::IsObject(env, eventInfo)) {
        NapiUtil::ThrowError(env, NapiError::ERR_PARAM, NapiUtil::CreateErrMsg("info", "AppEventInfo"), isV9_);
        return false;
    }
    return IsValidEventDomain(env, NapiUtil::GetProperty(env, eventInfo, DOMAIN_PROPERTY))
        && IsValidEventName(env, NapiUtil::GetProperty(env, eventInfo, NAME_PROPERTY))
        && IsValidEventType(env, NapiUtil::GetProperty(env, eventInfo, TYPE_PROPERTY))
        && IsValidEventParam(env, NapiUtil::GetProperty(env, eventInfo, PARAM_PROPERTY));
}

bool NapiHiAppEventBuilder::IsOldWriteParams(const napi_env env, const napi_value params[], size_t len)
{
    return IsValidEventName(env, params[0]) // 0 means the index of event name
        && IsValidEventType(env, params[1]) // 1 means the index of event type
        && IsValidEventParam(env, params[2]); // 2 means the index of event param
}

bool NapiHiAppEventBuilder::IsNewWriteParams(const napi_env env, const napi_value params[], size_t len)
{
    return IsValidEventInfo(env, params[0]);
}

void NapiHiAppEventBuilder::AddArrayParam2EventPack(napi_env env, const std::string &key,
    const napi_value arr)
{
    napi_valuetype type = NapiUtil::GetArrayType(env, arr);
    switch (type) {
        case napi_boolean: {
            std::vector<bool> bools;
            NapiUtil::GetBooleans(env, arr, bools);
            appEventPack_->AddParam(key, bools);
            break;
        }
        case napi_number: {
            std::vector<double> doubles;
            NapiUtil::GetDoubles(env, arr, doubles);
            appEventPack_->AddParam(key, doubles);
            break;
        }
        case napi_string: {
            std::vector<std::string> strs;
            NapiUtil::GetStrings(env, arr, strs);
            appEventPack_->AddParam(key, strs);
            break;
        }
        case napi_null: {
            appEventPack_->AddParam(key);
            break;
        }
        default: {
            HiLog::Error(LABEL, "array param value type is invalid");
            result_ = ERROR_INVALID_LIST_PARAM_TYPE;
            std::string errMsg = NapiUtil::CreateErrMsg("param value", PARAM_VALUE_TYPE);
            NapiUtil::ThrowError(env, NapiError::ERR_PARAM, errMsg, isV9_);
            break;
        }
    }
}

void NapiHiAppEventBuilder::AddParam2EventPack(napi_env env, const std::string &key,
    const napi_value value)
{
    napi_valuetype type = NapiUtil::GetType(env, value);
    switch (type) {
        case napi_boolean:
            appEventPack_->AddParam(key, NapiUtil::GetBoolean(env, value));
            break;
        case napi_number:
            appEventPack_->AddParam(key, NapiUtil::GetDouble(env, value));
            break;
        case napi_string:
            appEventPack_->AddParam(key, NapiUtil::GetString(env, value));
            break;
        case napi_object:
            if (NapiUtil::IsArray(env, value)) {
                AddArrayParam2EventPack(env, key, value);
                break;
            }
            [[fallthrough]];
        default:
            HiLog::Error(LABEL, "param value type is invalid");
            result_ = ERROR_INVALID_PARAM_VALUE_TYPE;
            std::string errMsg = NapiUtil::CreateErrMsg("param value", PARAM_VALUE_TYPE);
            NapiUtil::ThrowError(env, NapiError::ERR_PARAM, errMsg, isV9_);
            break;
    }
}

void NapiHiAppEventBuilder::AddParams2EventPack(napi_env env, const napi_value paramObj)
{
    std::vector<std::string> keys;
    NapiUtil::GetPropertyNames(env, paramObj, keys);
    for (auto key : keys) {
        if (key.length() > MAX_LENGTH_OF_PARAM_NAME) {
            result_ = ERROR_INVALID_PARAM_NAME;
            HiLog::Info(LABEL, "the length=%{public}zu of the param key is invalid", key.length());
            continue;
        }
        napi_value value = NapiUtil::GetProperty(env, paramObj, key);
        if (value == nullptr) {
            result_ = ERROR_INVALID_PARAM_VALUE_TYPE;
            std::string errMsg = NapiUtil::CreateErrMsg("param value", PARAM_VALUE_TYPE);
            NapiUtil::ThrowError(env, NapiError::ERR_PARAM, errMsg, isV9_);
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
    if (len < 3) { // the min number of params for write is 3
        result_ = ERROR_INVALID_PARAM_NUM_JS;
        return nullptr;
    }

    // 1. build AppEventPack
    if (IsOldWriteParams(env, params, len)) {
        BuildEventPack(env, params);
    } else {
        result_ = ERROR_INVALID_PARAM_TYPE_JS;
    }

    // 2. build callback if any
    BuildCallback(env, params[len - 1]); // (len - 1) means the last param
    return appEventPack_;
}

std::shared_ptr<AppEventPack> NapiHiAppEventBuilder::BuildV9(const napi_env env, const napi_value params[], size_t len)
{
    isV9_ = true;
    if (len < 1) { // the min number of params for writeV9 is 1
        NapiUtil::ThrowError(env, NapiError::ERR_PARAM, NapiUtil::CreateErrMsg("info"), isV9_);
        return nullptr;
    }
    if (!IsNewWriteParams(env, params, len)) {
        return nullptr;
    }
    BuildEventPack(env, params[0]);
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
