/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "hiappevent_userinfo.h"

#include <mutex>
#include <string>

#include "app_event_store.h"
#include "app_event_observer_mgr.h"
#include "hiappevent_base.h"
#include "hiappevent_verify.h"
#include "hilog/log.h"

namespace OHOS {
namespace HiviewDFX {
namespace HiAppEvent {

namespace {
const HiLogLabel LABEL = { LOG_CORE, HIAPPEVENT_DOMAIN, "HiAppEvent_userInfo" };

constexpr int DB_FAILED = -1;
constexpr int USER_ID_COUNT_MAX = 10;
constexpr int USER_PROPERTY_COUNT_MAX = 300;

std::mutex g_mutex;
}

UserInfo::UserInfo() : userIdVersion_(0), userPropertyVersion_(0)
{
    InitUserIds();
    InitUserProperties();
}

int UserInfo::SetUserId(const std::string& name, const std::string& value)
{
    HiLog::Debug(LABEL, "start to set userId, name=%{public}s, value=%{public}s.", name.c_str(), value.c_str());

    if (VerifyUserId(name, value) != 0) {
        HiLog::Error(LABEL, "userId data invalid.");
        return -1;
    }
    std::string out;
    if (AppEventStore::GetInstance().QueryUserId(name, out) == DB_FAILED) {
        HiLog::Warn(LABEL, "failed to insert user id, name=%{public}s.", name.c_str());
        return -1;
    }
    if (out.empty()) {
        if (userIds_.size() >= USER_ID_COUNT_MAX) {
            HiLog::Warn(LABEL, "failed to insert user id, name=%{public}s.", name.c_str());
            return -1;
        }
        if (AppEventStore::GetInstance().InsertUserId(name, value) == DB_FAILED) {
            HiLog::Warn(LABEL, "failed to insert user id, name=%{public}s.", name.c_str());
            return -1;
        }
    } else {
        if (AppEventStore::GetInstance().UpdateUserId(name, value) == DB_FAILED) {
            HiLog::Warn(LABEL, "failed to update user id, name=%{public}s.", name.c_str());
            return -1;
        }
    }
    userIds_[name] = value;
    userIdVersion_++;

    return 0;
}

int UserInfo::GetUserId(const std::string& name, std::string& out)
{
    HiLog::Debug(LABEL, "start to get userId, name=%{public}s.", name.c_str());

    if (VerifyUserId(name) != 0) {
        HiLog::Error(LABEL, "userId name invalid.");
        return -1;
    }
    if (userIds_.find(name) == userIds_.end()) {
        HiLog::Warn(LABEL, "failed to get userid, name=%{public}s.", name.c_str());
        return -1;
    }
    out = userIds_.at(name);

    return 0;
}

int UserInfo::RemoveUserId(const std::string& name)
{
    HiLog::Debug(LABEL, "start to remove userId, name=%{public}s.", name.c_str());

    if (VerifyUserId(name) != 0) {
        HiLog::Error(LABEL, "userId name invalid.");
        return -1;
    }
    if (AppEventStore::GetInstance().DeleteUserId(name) == DB_FAILED) {
        HiLog::Warn(LABEL, "failed to remove userid, name=%{public}s.", name.c_str());
        return -1;
    }
    if (userIds_.find(name) != userIds_.end()) {
        userIds_.erase(name);
        userIdVersion_++;
    }

    return 0;
}

int UserInfo::SetUserProperty(const std::string& name, const std::string& value)
{
    HiLog::Debug(LABEL, "start to set userProperty, name=%{public}s, value=%{public}s.", name.c_str(), value.c_str());

    if (VerifyUserProperty(name, value) != 0) {
        HiLog::Error(LABEL, "user property data invalid.");
        return -1;
    }
    std::string out;
    if (AppEventStore::GetInstance().QueryUserProperty(name, out) == DB_FAILED) {
        HiLog::Warn(LABEL, "failed to insert user property, name=%{public}s.", name.c_str());
        return -1;
    }
    if (out.empty()) {
        if (userProperties_.size() >= USER_PROPERTY_COUNT_MAX) {
            HiLog::Warn(LABEL, "failed to insert user property, name=%{public}s.", name.c_str());
            return -1;
        }
        if (AppEventStore::GetInstance().InsertUserProperty(name, value) == DB_FAILED) {
            HiLog::Warn(LABEL, "failed to insert user property, name=%{public}s.", name.c_str());
            return -1;
        }
    } else {
        if (AppEventStore::GetInstance().UpdateUserProperty(name, value) == DB_FAILED) {
            HiLog::Warn(LABEL, "failed to update user property, name=%{public}s.", name.c_str());
            return -1;
        }
    }
    userProperties_[name] = value;
    userPropertyVersion_++;

    return 0;
}

int UserInfo::GetUserProperty(const std::string& name, std::string& out)
{
    HiLog::Debug(LABEL, "start to get userProperty, name=%{public}s.", name.c_str());

    if (VerifyUserProperty(name) != 0) {
        HiLog::Error(LABEL, "user property name invalid.");
        return -1;
    }
    if (userProperties_.find(name) == userProperties_.end()) {
        HiLog::Warn(LABEL, "failed to get user property, name=%{public}s.", name.c_str());
        return -1;
    }
    out = userProperties_.at(name);

    return 0;
}

int UserInfo::RemoveUserProperty(const std::string& name)
{
    HiLog::Debug(LABEL, "start to remove userProperty, name=%{public}s.", name.c_str());

    if (VerifyUserProperty(name) != 0) {
        HiLog::Error(LABEL, "user property name invalid.");
        return -1;
    }
    if (AppEventStore::GetInstance().DeleteUserProperty(name) == DB_FAILED) {
        HiLog::Warn(LABEL, "failed to remove user property, name=%{public}s.", name.c_str());
        return -1;
    }
    if (userProperties_.find(name) != userProperties_.end()) {
        userProperties_.erase(name);
        userPropertyVersion_++;
    }

    return 0;
}

void UserInfo::InitUserIds()
{
    userIds_.clear();
    if (AppEventStore::GetInstance().QueryUserIds(userIds_) == DB_FAILED) {
        HiLog::Warn(LABEL, "failed to get user ids.");
        return;
    }
}

void UserInfo::InitUserProperties()
{
    userProperties_.clear();
    if (AppEventStore::GetInstance().QueryUserIds(userProperties_) == DB_FAILED) {
        HiLog::Warn(LABEL, "failed to get user properties.");
        return;
    }
}

std::vector<HiAppEvent::UserId> UserInfo::GetUserIds()
{
    std::vector<HiAppEvent::UserId> userIds;
    for (auto it = userIds_.begin(); it != userIds_.end(); it++) {
        HiAppEvent::UserId userId;
        userId.name = it->first;
        userId.value = it->second;
        userIds.emplace_back(userId);
    }
    return userIds;
}

std::vector<HiAppEvent::UserProperty> UserInfo::GetUserProperties()
{
    std::vector<HiAppEvent::UserProperty> userProperties;
    for (auto it = userProperties_.begin(); it != userProperties_.end(); it++) {
        HiAppEvent::UserProperty userProperty;
        userProperty.name = it->first;
        userProperty.value = it->second;
        userProperties.emplace_back(userProperty);
    }
    return userProperties;
}

int64_t UserInfo::GetUserIdVersion()
{
    return userIdVersion_;
}

int64_t UserInfo::GetUserPropertyVersion()
{
    return userPropertyVersion_;
}
} // namespace HiAppEvent
} // namespace HiviewDFX
} // namespace OHOS