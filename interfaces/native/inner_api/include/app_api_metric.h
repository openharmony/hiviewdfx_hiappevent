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
#ifndef HIAPPEVENT_INTERFACES_NATIVE_INNER_API_INCLUDE_APP_API_METRIC_H
#define HIAPPEVENT_INTERFACES_NATIVE_INNER_API_INCLUDE_APP_API_METRIC_H

#include "base_type.h"

namespace OHOS {
namespace HiviewDFX {
namespace HiAppEvent {

/**
 * @brief Reports API metric information.
 *
 * This interface is used to report API metric information including API info and metric data.
 * The reported data can be used for API performance monitoring and analysis.
 *
 * @param apiInfo API information including kit name and API name.
 * @param metric API metric information including error code, duration, and success status.
 * @return Returns 0 if the operation is successful; otherwise, returns a negative integer.
 * @warning This is an asynchronous interface and includes time-consuming operations. To ensure performance,
 *          avoid calling this interface frequently or continuously.
 */
int ReportApiMetric(const ApiInfo& apiInfo, const ApiMetric& metric);
} // namespace HiAppEvent
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIAPPEVENT_INTERFACES_NATIVE_INNER_API_INCLUDE_APP_API_METRIC_H
