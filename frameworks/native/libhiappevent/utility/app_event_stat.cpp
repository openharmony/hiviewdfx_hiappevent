/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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
#include "app_event_stat.h"

#include <random>

#include "hiappevent_base.h"
#include "hiappevent_write.h"
#include "time_util.h"

namespace OHOS {
namespace HiviewDFX {
namespace AppEventStat {
namespace {
constexpr int BEHAVIOR = 4;

uint64_t RandomNum()
{
    std::random_device seed;
    std::mt19937_64 gen(seed());
    std::uniform_int_distribution<uint64_t> dis(0, std::numeric_limits<uint64_t>::max());
    return dis(gen);
}
}

void WriteApiEndEventAsync(const std::string& apiName, uint64_t beginTime, int result, int errCode)
{
    auto appEventPack = std::make_shared<AppEventPack>("api_diagnostic", "api_exec_end", BEHAVIOR);
    appEventPack->AddParam("trans_id", "transId_" + std::to_string(RandomNum()));
    appEventPack->AddParam("api_name", apiName);
    appEventPack->AddParam("sdk_name", "PerformanceAnalysisKit");
    int64_t costTime = TimeUtil::GetElapsedMilliSecondsSinceBoot() - static_cast<int64_t>(beginTime);
    int64_t realEndTime = TimeUtil::GetMilliSecondsTimestamp(CLOCK_REALTIME);
    int64_t realBeginTime = realEndTime - costTime;
    if (realBeginTime < 0 || realEndTime < 0) {
        return;
    }
    appEventPack->AddParam("begin_time", realBeginTime);
    appEventPack->AddParam("end_time", realEndTime);
    appEventPack->AddParam("result", result);
    appEventPack->AddParam("error_code", errCode);
    SubmitWritingTask(appEventPack, "appevent_api_end");
}
} // namespace AppEventStat
} // namespace HiviewDFX
} // namespace OHOS
