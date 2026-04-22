/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with License.
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

#include "api_stats_mgr.h"

#include "hiappevent_base.h"
#include "hiappevent_write.h"
#include "hilog/log.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD002D07

#undef LOG_TAG
#define LOG_TAG "ApiStatsMgr"

namespace OHOS {
namespace HiviewDFX {
namespace HiAppEvent {

ApiStatsManager::ApiStatsManager()
{
    ScheduleReport();
    timer_.SetBackUpCallback([this]() {
        this->ScheduleBackUp();
    });
    timer_.SetReportCallback([this]() {
        this->ScheduleReport();
    });
    timer_.Start();
}

ApiStatsManager::~ApiStatsManager()
{
    timer_.Stop();
}

void ApiStatsManager::AddRecord(ApiDescriptor descriptor, ApiMetric metric)
{
    HILOG_DEBUG(LOG_CORE, "AddRecord: kitName=%{public}s, apiName=%{public}s",
        descriptor.KitName().c_str(), descriptor.ApiName().c_str());
    std::lock_guard<std::mutex> lock(mut_);
    aggregator_.Record(descriptor, metric);
}

void ApiStatsManager::ScheduleBackUpInner()
{
    if (!aggregator_.IsUpdatedAfterLastBackup()) {
        HILOG_INFO(LOG_CORE, "ScheduleBackUpInner: no update, skip");
        return;
    }

    auto apiMetrics = aggregator_.GetApiMetrics();
    HILOG_INFO(LOG_CORE, "ScheduleBackUpInner: backup count=%{public}zu", apiMetrics.size());
    if (ApiStatsStorage::GetInstance().Backup(apiMetrics) == 0) {
        aggregator_.ClearRecord();
        HILOG_DEBUG(LOG_CORE, "ScheduleBackUpInner success");
    } else {
        HILOG_ERROR(LOG_CORE, "ScheduleBackUpInner failed to backup api stats");
    }
}

void ApiStatsManager::ScheduleBackUp()
{
    HILOG_DEBUG(LOG_CORE, "ScheduleBackUp start");
    std::lock_guard<std::mutex> lock(mut_);
    ApiStatsManager::ScheduleBackUpInner();
}

void ApiStatsManager::ScheduleReport()
{
    HILOG_DEBUG(LOG_CORE, "ScheduleReport start");
    std::lock_guard<std::mutex> lock(mut_);
    ApiStatsManager::ScheduleBackUpInner();
    
    ApiMetricsMap apiMetrics;
    if (ApiStatsStorage::GetInstance().QueryAll(apiMetrics) != 0) {
        HILOG_ERROR(LOG_CORE, "failed to query api stats for report");
        return;
    }
    
    HILOG_DEBUG(LOG_CORE, "ScheduleReport: query count=%{public}zu", apiMetrics.size());
    auto reports = ApiStatsAggregator::AggregateStats(apiMetrics);
    
    ReportStats(reports);
    
    if (ApiStatsStorage::GetInstance().Clear() != 0) {
        HILOG_ERROR(LOG_CORE, "failed to clear api stats after report");
    } else {
        HILOG_DEBUG(LOG_CORE, "ScheduleReport: clear success");
    }
}

std::shared_ptr<AppEventPack> ApiStatsManager::ConvertReportToEventPack(const ApiStatsReport& report)
{
    constexpr int BEHAVIOR = 4;
    auto appEventPack = std::make_shared<AppEventPack>("api_diagnostic", "api_called_stat", BEHAVIOR);
    
    appEventPack->AddParam("api_name", report.apiName);
    appEventPack->AddParam("sdk_name", report.kitName);
    appEventPack->AddParam("begin_time", report.begin_time);
    appEventPack->AddParam("call_times", report.call_times);
    appEventPack->AddParam("success_times", report.success_times);
    appEventPack->AddParam("max_cost_time", report.max_cost_time);
    appEventPack->AddParam("min_cost_time", report.min_cost_time);
    appEventPack->AddParam("total_cost_time", report.total_cost_time);
    
    std::vector<int> errorCodeTypes;
    std::vector<int> errorCounts;
    for (size_t i = 0; i < report.error_code_types.size(); i++) {
        errorCodeTypes.push_back(report.error_code_types[i]);
        errorCounts.push_back(report.error_code_num[i]);
    }
    appEventPack->AddParam("error_code_types", errorCodeTypes);
    appEventPack->AddParam("error_code_num", errorCounts);

    return appEventPack;
}

void ApiStatsManager::ReportStats(const std::vector<ApiStatsReport>& reports)
{
    if (reports.empty()) {
        HILOG_DEBUG(LOG_CORE, "no api stats to report");
        return;
    }
    
    HILOG_INFO(LOG_CORE, "start to report api stats, count=%{public}zu", reports.size());
    
    for (const auto& report : reports) {
        auto eventPack = ConvertReportToEventPack(report);
        SubmitWritingTask(eventPack, "api_stats_report");
        HILOG_INFO(LOG_CORE, "report api stats: kitName=%{public}s, apiName=%{public}s, call_times=%{public}d, "
            "success_times=%{public}d, max_cost_time=%{public}d, min_cost_time=%{public}d, "
            "total_cost_time=%{public}d, error_code_count=%{public}zu",
            report.kitName.c_str(), report.apiName.c_str(), report.call_times, report.success_times,
            report.max_cost_time, report.min_cost_time, report.total_cost_time, report.error_code_types.size());
    }
    
    HILOG_INFO(LOG_CORE, "report api stats success");
}

} // namespace HiAppEvent
} // namespace HiviewDFX
} // namespace OHOS