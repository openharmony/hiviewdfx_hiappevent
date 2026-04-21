/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "hiappevent_api_metric.h"

#include <string>

#include "hiappevent_base.h"
#include "hilog/log.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD002D07

#undef LOG_TAG
#define LOG_TAG "ApiMetric"

namespace OHOS {
namespace HiviewDFX {
namespace HiAppEvent {

ApiMetricProcessor& ApiMetricProcessor::GetInstance()
{
    static ApiMetricProcessor apiMetricProcessor;
    return apiMetricProcessor;
}

int ApiMetricProcessor::ProcessApiMetric(const HiAppEvent::ApiInfo& apiInfo, const HiAppEvent::ApiMetric& metric)
{
    HILOG_DEBUG(LOG_CORE, "ProcessApiMetric: kitName=%{public}s, apiName=%{public}s",
        apiInfo.kit.c_str(), apiInfo.api.c_str());

    if (metric.duration < 0 || apiInfo.kit.empty() || apiInfo.api.empty()) {
        return ErrorCode::ERROR_INVALID_PARAM_VALUE;
    }
    ApiDescriptor descriptor(apiInfo.kit, apiInfo.api);
    apiStatsMgr.AddRecord(descriptor, metric);
    return ErrorCode::HIAPPEVENT_VERIFY_SUCCESSFUL;
}

} // namespace HiAppEvent
} // namespace HiviewDFX
} // namespace OHOS
