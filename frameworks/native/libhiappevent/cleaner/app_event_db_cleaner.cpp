/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include "app_event_db_cleaner.h"

#include "app_event_store.h"
#include "file_util.h"
#include "hilog/log.h"

namespace OHOS {
namespace HiviewDFX {
namespace {
const HiLogLabel LABEL = { LOG_CORE, 0xD002D07, "HiAppEvent_DbCleaner" };
const std::string DATABASE_DIR = "databases/";
}
uint64_t AppEventDbCleaner::GetFilesSize()
{
    return FileUtil::GetDirSize(path_ + DATABASE_DIR);
}

uint64_t AppEventDbCleaner::ClearSpace(uint64_t curSize, uint64_t maxSize)
{
    HiLog::Info(LABEL, "start to clear the space occupied by database files");
    if (curSize <= maxSize) {
        return curSize;
    }
    (void)AppEventStore::GetInstance().DestroyDbStore();
    return 0;
}

void AppEventDbCleaner::ClearData()
{
    HiLog::Info(LABEL, "start to clear the db data");
    (void)AppEventStore::GetInstance().DestroyDbStore();
}
} // namespace HiviewDFX
} // namespace OHOS
