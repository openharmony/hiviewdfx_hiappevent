/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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
#ifndef HIAPPEVENT_FRAMEWORKS_JS_NAPI_INCLUDE_NAPI_HIAPPEVENT_PROCESSOR_H
#define HIAPPEVENT_FRAMEWORKS_JS_NAPI_INCLUDE_NAPI_HIAPPEVENT_PROCESSOR_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace HiviewDFX {
namespace NapiHiAppEventProcessor {
struct AddProcessorFromConfigAsyncContext {
    napi_async_work asyncWork {nullptr};
    napi_deferred deferred {nullptr};
    std::string processorName;
    std::string configName {"SDK_OCG"};
    int64_t result {0};
};

bool AddProcessor(const napi_env env, const napi_value config, napi_value& out);
void AddProcessorFromConfig(const napi_env, AddProcessorFromConfigAsyncContext* asyncContext);
bool RemoveProcessor(const napi_env env, const napi_value id);
} // namespace NapiHiAppEventProcessor
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIAPPEVENT_FRAMEWORKS_JS_NAPI_INCLUDE_NAPI_HIAPPEVENT_PROCESSOR_H
