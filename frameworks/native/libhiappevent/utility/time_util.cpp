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
#include "time_util.h"

#include <chrono>
#include <ctime>
#include <sys/time.h>

namespace OHOS {
namespace HiviewDFX {
namespace TimeUtil {
namespace {
const std::string DEFAULT_DATE = "19700101";
}
uint64_t GetMilliseconds()
{
    auto now = std::chrono::system_clock::now();
    auto millisecs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return millisecs.count();
}

std::string GetDate()
{
    time_t nowTime = time(nullptr);
    if (nowTime < 0) {
        return DEFAULT_DATE;
    }
    char dateChs[9] = { 0 }; // 9 means 8(19700101) + 1('\0')
    struct tm localTm;
    if (localtime_noenv_r(&nowTime, &localTm) == nullptr) {
        return DEFAULT_DATE;
    }
    if (strftime(dateChs, sizeof(dateChs), "%Y%m%d", &localTm) == 0) {
        return DEFAULT_DATE;
    }
    return dateChs;
}

std::string GetTimeZone()
{
    struct timeval tv;
    if (gettimeofday(&tv, nullptr) != 0) {
        return "";
    }
    time_t sysSec = tv.tv_sec;
    struct tm tmLocal;
    if (localtime_noenv_r(&sysSec, &tmLocal) == nullptr) {
        return "";
    }
    constexpr size_t buffSize = 6; // for '+0800\0'
    char buff[buffSize] = {0};
    if (strftime(buff, sizeof(buff) - 1, "%z", &tmLocal) == 0) {
        return "";
    }
    return std::string(buff);
}

int64_t GetMilliSecondsTimestamp(clockid_t clockId)
{
    struct timespec times {};
    if (clock_gettime(clockId, &times) == -1) {
        return -1;
    }
    constexpr int64_t secondToMillisecond = 1 * 1000;
    constexpr int64_t nanosecondToMillisecond = 1 * 1000 * 1000;
    return times.tv_sec * secondToMillisecond + times.tv_nsec / nanosecondToMillisecond;
}

int64_t GetElapsedMilliSecondsSinceBoot()
{
    return GetMilliSecondsTimestamp(CLOCK_BOOTTIME);
}
} // namespace TimeUtil
} // namespace HiviewDFX
} // namespace OHOS
