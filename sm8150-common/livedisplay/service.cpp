/*
 * Copyright (C) 2019 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <dlfcn.h>

#define LOG_TAG "vendor.lineage.livedisplay@2.1-service-lge_sm8150"

#include <android-base/logging.h>
#include <binder/ProcessState.h>
#include <hidl/HidlTransportSupport.h>
#include <livedisplay/sdm/PictureAdjustment.h>
#include <vendor/lineage/livedisplay/2.0/IPictureAdjustment.h>

#include "AntiFlicker.h"
#include "DisplayModes.h"
#include "PictureAdjustment.h"
#include "SunlightEnhancement.h"

constexpr const char* SDM_DISP_LIBS[]{
        "libsdm-disp-vndapis.so",
};

using android::OK;
using android::sp;
using android::status_t;
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;

using ::vendor::lineage::livedisplay::V2_0::sdm::SDMController;
using ::vendor::lineage::livedisplay::V2_1::IAntiFlicker;
using ::vendor::lineage::livedisplay::V2_1::IDisplayModes;
using ::vendor::lineage::livedisplay::V2_0::IPictureAdjustment;
using ::vendor::lineage::livedisplay::V2_1::ISunlightEnhancement;
using ::vendor::lineage::livedisplay::V2_1::implementation::AntiFlicker;
using ::vendor::lineage::livedisplay::V2_1::implementation::DisplayModes;
using ::vendor::lineage::livedisplay::V2_0::implementation::PictureAdjustment;
using ::vendor::lineage::livedisplay::V2_1::implementation::SunlightEnhancement;

int main() {
    void* libHandle = nullptr;
    const char* libName = nullptr;
    int32_t (*disp_api_init)(uint64_t*, uint32_t) = nullptr;
    int32_t (*disp_api_deinit)(uint64_t, uint32_t) = nullptr;
    uint64_t cookie = 0;
    status_t status = OK;

    android::ProcessState::initWithDriver("/dev/vndbinder");

    LOG(INFO) << "LiveDisplay HAL service is starting.";

    std::shared_ptr<SDMController> controller = std::make_shared<SDMController>();
    sp<AntiFlicker> af = new AntiFlicker();
    sp<DisplayModes> dm = new DisplayModes(controller);
    sp<SunlightEnhancement> se = new SunlightEnhancement();
    sp<PictureAdjustment> pa;

    for (auto&& lib : SDM_DISP_LIBS) {
        libHandle = dlopen(lib, RTLD_NOW);
        libName = lib;
        if (libHandle != nullptr) {
            LOG(INFO) << "Loaded: " << libName;
            break;
        }
        LOG(ERROR) << "Can not load " << libName << " (" << dlerror() << ")";
    }

    if (libHandle == nullptr) {
        LOG(ERROR) << "Failed to load SDM display lib, exiting.";
        goto shutdown;
    }

    disp_api_init =
            reinterpret_cast<int32_t (*)(uint64_t*, uint32_t)>(dlsym(libHandle, "disp_api_init"));
    if (disp_api_init == nullptr) {
        LOG(ERROR) << "Can not get disp_api_init from " << libName << " (" << dlerror() << ")";
        goto shutdown;
    }

    disp_api_deinit =
            reinterpret_cast<int32_t (*)(uint64_t, uint32_t)>(dlsym(libHandle, "disp_api_deinit"));
    if (disp_api_deinit == nullptr) {
        LOG(ERROR) << "Can not get disp_api_deinit from " << libName << " (" << dlerror() << ")";
        goto shutdown;
    }

    status = disp_api_init(&cookie, 0);
    if (status != OK) {
        LOG(ERROR) << "Can not initialize " << libName << " (" << status << ")";
        goto shutdown;
    }

    pa = new PictureAdjustment(libHandle, cookie);
    if (pa == nullptr) {
        LOG(ERROR) << "Can not create an instance of LiveDisplay HAL PictureAdjustment Iface,"
                   << " exiting.";
        goto shutdown;
    }

    if (!pa->isSupported()) {
        // Backend isn't ready yet, so restart and try again
        goto shutdown;
    }

    configureRpcThreadpool(1, true /*callerWillJoin*/);

    status = af->registerAsService();
    if (status != OK) {
        LOG(ERROR) << "Could not register service for LiveDisplay HAL AntiFlicker Iface ("
                   << status << ")";
        goto shutdown;
    }

    status = dm->registerAsService();
    if (status != OK) {
        LOG(ERROR) << "Could not register service for LiveDisplay HAL DisplayModes Iface ("
                   << status << ")";
        goto shutdown;
    }

    status = pa->registerAsService();
    if (status != OK) {
        LOG(ERROR) << "Could not register service for LiveDisplay HAL PictureAdjustment Iface ("
                   << status << ")";
        goto shutdown;
    }

    status = se->registerAsService();
    if (status != OK) {
        LOG(ERROR) << "Could not register service for LiveDisplay HAL SunlightEnhancement Iface ("
                   << status << ")";
        goto shutdown;
    }

    LOG(INFO) << "LiveDisplay HAL service is ready.";
    joinRpcThreadpool();
    // Should not pass this line

shutdown:
    // In normal operation, we don't expect the thread pool to shutdown
    LOG(ERROR) << "LiveDisplay HAL service is shutting down.";
    return 1;
}
