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

#ifndef HI_APP_EVENT_VERIFY_H
#define HI_APP_EVENT_VERIFY_H

#include <memory>
#include <string>

namespace OHOS {
namespace HiviewDFX {
class AppEventPack;
bool IsValidDomain(const std::string& domain);
bool IsValidWatcherName(const std::string& watcherName);
bool IsValidEventType(int eventType);
int VerifyAppEvent(std::shared_ptr<AppEventPack>& appEventPack);
int VerifyUserId(const std::string& name);
int VerifyUserId(const std::string& name, const std::string& value);
int VerifyUserProperty(const std::string& name);
int VerifyUserProperty(const std::string& name, const std::string& value);
} // namespace HiviewDFX
} // namespace OHOS
#endif // HI_APP_EVENT_VERIFY_H
