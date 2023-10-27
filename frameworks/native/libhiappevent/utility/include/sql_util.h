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
#ifndef HIAPPEVENT_FRAMEWORKS_NATIVE_LIB_HIAPPEVENT_UTILITY_SQL_UTIL_H
#define HIAPPEVENT_FRAMEWORKS_NATIVE_LIB_HIAPPEVENT_UTILITY_SQL_UTIL_H

#include <string>

namespace OHOS {
namespace HiviewDFX {
namespace SqlUtil {
const std::string SQL_TEXT_TYPE = "TEXT NOT NULL";
const std::string SQL_INT_TYPE = "INTEGER";

std::string CreateTable(const std::string& table,
    const std::vector<std::pair<std::string, std::string>>& fields);
std::string DropTable(const std::string& table);
} // namespace SqlUtil
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIAPPEVENT_FRAMEWORKS_NATIVE_LIB_HIAPPEVENT_UTILITY_SQL_UTIL_H