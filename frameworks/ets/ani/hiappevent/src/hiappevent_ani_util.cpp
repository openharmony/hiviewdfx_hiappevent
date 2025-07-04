/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "hiappevent_ani_util.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD002D07

#undef LOG_TAG
#define LOG_TAG "HIAPPEVENT_ANI_UTIL"

using namespace OHOS::HiviewDFX;
namespace {
const std::pair<const char*, AniArgsType> OBJECT_TYPE[] = {
    {CLASS_NAME_INT, AniArgsType::ANI_INT},
    {CLASS_NAME_BOOLEAN, AniArgsType::ANI_BOOLEAN},
    {CLASS_NAME_DOUBLE, AniArgsType::ANI_NUMBER},
    {CLASS_NAME_STRING, AniArgsType::ANI_STRING},
};
}

std::string HiAppEventAniUtil::CreateErrMsg(const std::string& name)
{
    return "Parameter error. The " + name + " parameter is mandatory.";
}

bool HiAppEventAniUtil::IsArray(ani_env *env, ani_object object)
{
    ani_boolean IsArray = ANI_FALSE;
    ani_class cls {};
    if (env->FindClass(CLASS_NAME_ARRAY, &cls) != ANI_OK) {
        return false;
    }
    ani_static_method static_method {};
    if (env->Class_FindStaticMethod(cls, "isArray", nullptr, &static_method)  != ANI_OK) {
        return false;
    }
    if (env->Class_CallStaticMethod_Boolean(cls, static_method, &IsArray, object) != ANI_OK) {
        return false;
    }
    return static_cast<bool>(IsArray);
}

bool HiAppEventAniUtil::IsRefUndefined(ani_env *env, ani_ref ref)
{
    ani_boolean isUndefined = ANI_FALSE;
    env->Reference_IsUndefined(ref, &isUndefined);
    return isUndefined;
}

void HiAppEventAniUtil::ParseRecord(ani_env *env, ani_ref recordRef, std::map<std::string, ani_ref>& recordResult)
{
    ani_ref keys {};
    if (env->Object_CallMethodByName_Ref(static_cast<ani_object>(recordRef), "keys",
                                         ":Lescompat/IterableIterator;", &keys) != ANI_OK) {
        HILOG_ERROR(LOG_CORE, "call method keys() failed.");
    }
    ani_boolean done = ANI_FALSE;
    while (!done) {
        ani_ref next {};
        if (env->Object_CallMethodByName_Ref(static_cast<ani_object>(keys), "next", nullptr, &next) != ANI_OK) {
            HILOG_ERROR(LOG_CORE, "call method next() failed.");
            break;
        }
        if (env->Object_GetFieldByName_Boolean(static_cast<ani_object>(next), "done", &done) != ANI_OK) {
            HILOG_ERROR(LOG_CORE, "get field done failed.");
            break;
        }
        if (done) {
            break;
        }
        ani_ref keyRef {};
        if (env->Object_GetFieldByName_Ref(static_cast<ani_object>(next), "value", &keyRef) != ANI_OK) {
            HILOG_ERROR(LOG_CORE, "get field value failed.");
            break;
        }
        ani_ref valueRef {};
        if (env->Object_CallMethodByName_Ref(static_cast<ani_object>(recordRef),
                                             "$_get", nullptr, &valueRef, keyRef) != ANI_OK) {
            HILOG_ERROR(LOG_CORE, "call method $_get failed.");
            break;
        }
        std::string keyStr = ParseStringValue(env, keyRef);
        recordResult[keyStr] = valueRef;
    }
}

std::string HiAppEventAniUtil::ParseStringValue(ani_env *env, ani_ref aniStrRef)
{
    ani_size strSize = 0;
    if (env->String_GetUTF8Size(static_cast<ani_string>(aniStrRef), &strSize) != ANI_OK) {
        HILOG_ERROR(LOG_CORE, "get ani string size failed");
        return "";
    }
    std::vector<char> buffer(strSize + 1);
    char* utf8Buffer = buffer.data();
    ani_size bytesWritten = 0;
    if (env->String_GetUTF8(static_cast<ani_string>(aniStrRef), utf8Buffer, strSize + 1, &bytesWritten) != ANI_OK) {
        HILOG_ERROR(LOG_CORE, "get ani string failed");
        return "";
    }
    utf8Buffer[bytesWritten] = '\0';
    std::string content = std::string(utf8Buffer);
    return content;
}

int32_t HiAppEventAniUtil::ParseIntValue(ani_env *env, ani_ref elementRef)
{
    ani_int intVal = 0;
    ani_class cls {};
    ani_method unboxedMethod {};
    if (env->FindClass(CLASS_NAME_INT, &cls)) {
        HILOG_ERROR(LOG_CORE, "find class %{public}s failed", CLASS_NAME_INT);
        return static_cast<int32_t>(intVal);
    }
    if (env->Class_FindMethod(cls, FUNC_NAME_UNBOXED, ":I", &unboxedMethod)  != ANI_OK) {
        HILOG_ERROR(LOG_CORE, "find method %{public}s failed", FUNC_NAME_UNBOXED);
        return static_cast<int32_t>(intVal);
    }
    if (env->Object_CallMethod_Int(static_cast<ani_object>(elementRef), unboxedMethod, &intVal) != ANI_OK) {
        HILOG_ERROR(LOG_CORE, "call method %{public}s failed", FUNC_NAME_UNBOXED);
        return static_cast<int32_t>(intVal);
    }
    return static_cast<int32_t>(intVal);
}

bool HiAppEventAniUtil::ParseBoolValue(ani_env *env, ani_ref elementRef)
{
    ani_class cls {};
    if (env->FindClass(CLASS_NAME_BOOLEAN, &cls) != ANI_OK) {
        HILOG_ERROR(LOG_CORE, "find class %{public}s failed", CLASS_NAME_BOOLEAN);
        return false;
    }
    ani_method unboxedMethod {};
    if (env->Class_FindMethod(cls, FUNC_NAME_UNBOXED, ":Z", &unboxedMethod) != ANI_OK) {
        HILOG_ERROR(LOG_CORE, "find method %{public}s failed", FUNC_NAME_UNBOXED);
        return false;
    }
    ani_boolean booleanVal = static_cast<ani_boolean>(false);
    if (env->Object_CallMethod_Boolean(static_cast<ani_object>(elementRef), unboxedMethod, &booleanVal) != ANI_OK) {
        HILOG_ERROR(LOG_CORE, "call method %{public}s failed", FUNC_NAME_UNBOXED);
        return false;
    }
    return static_cast<bool>(booleanVal);
}

double HiAppEventAniUtil::ParseNumberValue(ani_env *env, ani_ref elementRef)
{
    ani_double doubleVal = 0;
    ani_class cls {};
    if (env->FindClass(CLASS_NAME_DOUBLE, &cls) != ANI_OK) {
        HILOG_ERROR(LOG_CORE, "find class %{public}s failed", CLASS_NAME_DOUBLE);
        return static_cast<double>(doubleVal);
    }
    ani_method unboxedMethod {};
    if (env->Class_FindMethod(cls, FUNC_NAME_UNBOXED, ":D", &unboxedMethod) != ANI_OK) {
        HILOG_ERROR(LOG_CORE, "find method %{public}s failed", FUNC_NAME_UNBOXED);
    }
    if (env->Object_CallMethod_Double(static_cast<ani_object>(elementRef), unboxedMethod, &doubleVal) != ANI_OK) {
        HILOG_ERROR(LOG_CORE, "call method %{public}s failed", FUNC_NAME_UNBOXED);
    }
    return static_cast<double>(doubleVal);
}

void HiAppEventAniUtil::GetStringsToSet(ani_env *env, ani_ref Ref, std::unordered_set<std::string> &arr)
{
    ani_size length = 0;
    if (env->Array_GetLength(static_cast<ani_array_ref>(Ref), &length) != ANI_OK) {
        HILOG_ERROR(LOG_CORE, "get array length failed");
    }
    for (ani_size i = 0; i < length; i++) {
        ani_ref value {};
        if (env->Array_Get_Ref(static_cast<ani_array_ref>(Ref), i, &value) != ANI_OK) {
            HILOG_ERROR(LOG_CORE, "get array element failed");
            continue;
        }
        arr.insert(ParseStringValue(env, static_cast<ani_string>(value)));
    }
}

ani_ref HiAppEventAniUtil::GetProperty(ani_env *env, ani_object object, const std::string &name)
{
    ani_ref value = nullptr;
    if (env->Object_GetPropertyByName_Ref(object, name.c_str(), &value) != ANI_OK) {
        HILOG_ERROR(LOG_CORE, "failed to get property %{public}s from object", name.c_str());
    }
    return value;
}

void HiAppEventAniUtil::ThrowAniError(ani_env *env, int32_t code, const std::string &message)
{
    ani_class cls {};
    if (env->FindClass(CLASS_NAME_BUSINESSERROR, &cls) != ANI_OK) {
        HILOG_ERROR(LOG_CORE, "find class %{public}s failed", CLASS_NAME_BUSINESSERROR);
        return;
    }
    ani_method ctor {};
    if (env->Class_FindMethod(cls, "<ctor>", ":V", &ctor) != ANI_OK) {
        HILOG_ERROR(LOG_CORE, "find method BusinessError constructor failed");
        return;
    }
    ani_object error {};
    if (env->Object_New(cls, ctor, &error) != ANI_OK) {
        HILOG_ERROR(LOG_CORE, "new object %{public}s failed", CLASS_NAME_BUSINESSERROR);
        return;
    }
    if (env->Object_SetPropertyByName_Double(error, "code", static_cast<ani_double>(code)) != ANI_OK) {
        HILOG_ERROR(LOG_CORE, "set property BusinessError.code failed");
        return;
    }
    ani_string messageRef {};
    if (env->String_NewUTF8(message.c_str(), message.size(), &messageRef) != ANI_OK) {
        HILOG_ERROR(LOG_CORE, "new message string failed");
        return;
    }
    if (env->Object_SetPropertyByName_Ref(error, "message", static_cast<ani_ref>(messageRef)) != ANI_OK) {
        HILOG_ERROR(LOG_CORE, "set property BusinessError.message failed");
        return;
    }
    if (env->ThrowError(static_cast<ani_error>(error)) != ANI_OK) {
        HILOG_ERROR(LOG_CORE, "throwError ani_error object failed");
    }
}

AniArgsType HiAppEventAniUtil::GetArgType(ani_env *env, ani_object elementObj)
{
    if (env == nullptr) {
        return AniArgsType::ANI_UNKNOWN;
    }
    if (HiAppEventAniUtil::IsRefUndefined(env, static_cast<ani_ref>(elementObj))) {
        return AniArgsType::ANI_UNDEFINED;
    }
    for (const auto &objType : OBJECT_TYPE) {
        ani_class cls {};
        if (env->FindClass(objType.first, &cls) != ANI_OK) {
            continue;
        }
        ani_boolean isInstance = ANI_FALSE;
        if (env->Object_InstanceOf(elementObj, cls, &isInstance) != ANI_OK) {
            continue;
        }
        if (static_cast<bool>(isInstance)) {
            return objType.second;
        }
    }
    return AniArgsType::ANI_UNKNOWN;
}

AniArgsType HiAppEventAniUtil::GetArrayType(ani_env *env, ani_ref arrayRef)
{
    if (env == nullptr) {
        return AniArgsType::ANI_UNKNOWN;
    }
    ani_size len = 0;
    if (env->Array_GetLength(static_cast<ani_array_ref>(arrayRef), &len) != ANI_OK) {
        HILOG_ERROR(LOG_CORE, "failed to get the length of array");
        return AniArgsType::ANI_UNDEFINED;
    }
    AniArgsType type = AniArgsType::ANI_NULL; // note: empty array returns null type
    for (ani_size i = 0; i < len; ++i) {
        ani_ref element = nullptr;
        if (env->Array_Get_Ref(static_cast<ani_array_ref>(arrayRef), i, &element) != ANI_OK) {
            HILOG_ERROR(LOG_CORE, "failed to get the element of array");
            return AniArgsType::ANI_UNDEFINED;
        }
        if (i == 0) {
            type = GetArgType(env, static_cast<ani_object>(element));
            continue;
        }
        if (type != GetArgType(env, static_cast<ani_object>(element))) {
            HILOG_ERROR(LOG_CORE, "array has different element types");
            return AniArgsType::ANI_UNDEFINED;
        }
    }
    return type;
}

std::vector<bool> HiAppEventAniUtil::GetBooleans(ani_env *env, ani_ref arrayRef)
{
    std::vector<bool> bools;
    ani_size len = 0;
    if (env->Array_GetLength(static_cast<ani_array_ref>(arrayRef), &len) != ANI_OK) {
        HILOG_ERROR(LOG_CORE, "failed to get the length of array");
        return bools;
    }
    for (ani_size i = 0; i < len; i++) {
        ani_ref element = nullptr;
        if (env->Array_Get_Ref(static_cast<ani_array_ref>(arrayRef), i, &element) != ANI_OK) {
            HILOG_ERROR(LOG_CORE, "failed to get the element of array");
            break;
        }
        bools.emplace_back(HiAppEventAniUtil::ParseBoolValue(env, element));
    }
    return bools;
}

std::vector<double> HiAppEventAniUtil::GetDoubles(ani_env *env, ani_ref arrayRef)
{
    std::vector<double> doubles;
    ani_size len = 0;
    if (env->Array_GetLength(static_cast<ani_array_ref>(arrayRef), &len) != ANI_OK) {
        HILOG_ERROR(LOG_CORE, "failed to get the length of array");
        return doubles;
    }
    for (ani_size i = 0; i < len; i++) {
        ani_ref element = nullptr;
        if (env->Array_Get_Ref(static_cast<ani_array_ref>(arrayRef), i, &element) != ANI_OK) {
            HILOG_ERROR(LOG_CORE, "failed to get the element of array");
            break;
        }
        doubles.emplace_back(HiAppEventAniUtil::ParseNumberValue(env, element));
    }
    return doubles;
}

std::vector<std::string> HiAppEventAniUtil::GetStrings(ani_env *env, ani_ref arrayRef)
{
    std::vector<std::string> strs;
    ani_size len = 0;
    if (env->Array_GetLength(static_cast<ani_array_ref>(arrayRef), &len) != ANI_OK) {
        HILOG_ERROR(LOG_CORE, "failed to get the length of array");
        return strs;
    }
    for (ani_size i = 0; i < len; i++) {
        ani_ref element = nullptr;
        if (env->Array_Get_Ref(static_cast<ani_array_ref>(arrayRef), i, &element) != ANI_OK) {
            HILOG_ERROR(LOG_CORE, "failed to get the element of array");
            break;
        }
        strs.emplace_back(HiAppEventAniUtil::ParseStringValue(env, element));
    }
    return strs;
}

std::vector<int> HiAppEventAniUtil::GetInts(ani_env *env, ani_ref arrayRef)
{
    std::vector<int> ints;
    ani_size len = 0;
    if (env->Array_GetLength(static_cast<ani_array_ref>(arrayRef), &len) != ANI_OK) {
        HILOG_ERROR(LOG_CORE, "failed to get the length of array");
        return ints;
    }
    for (ani_size i = 0; i < len; i++) {
        ani_ref element = nullptr;
        if (env->Array_Get_Ref(static_cast<ani_array_ref>(arrayRef), i, &element) != ANI_OK) {
            HILOG_ERROR(LOG_CORE, "failed to get the element of array");
            break;
        }
        ints.emplace_back(HiAppEventAniUtil::ParseIntValue(env, element));
    }
    return ints;
}
