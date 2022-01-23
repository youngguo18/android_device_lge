LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := RemovePkgs
LOCAL_MODULE_CLASS := APPS
LOCAL_MODULE_TAGS := optional
LOCAL_OVERRIDES_PACKAGES := AmbientSensePrebuilt AppDirectedSMSService BetterBug ConnMO DCMO DeviceIntelligenceNetworkPrebuilt
LOCAL_OVERRIDES_PACKAGES += DevicePolicyPrebuilt DiagnosticsToolPrebuilt DMService GCS HelpRtcPrebuilt LocationHistoryPrebuilt
LOCAL_OVERRIDES_PACKAGES += MaestroPrebuilt Maps MarkupGoogle MicropaperPrebuilt MyVerizonServices OBDM_Permissions OemDmTrigger
LOCAL_OVERRIDES_PACKAGES += RecorderPrebuilt SafetyHubPrebuilt SCONE ScribePrebuilt SecurityHubPrebuilt Showcase Snap SoundAmplifierPrebuilt
LOCAL_OVERRIDES_PACKAGES += SprintDM SprintHM TurboPrebuilt Tycho USCCDM VZWAPNLib VzwOmaTrigger WellbeingPrebuilt arcore obdm_stub
LOCAL_UNINSTALLABLE_MODULE := true
LOCAL_CERTIFICATE := platform
LOCAL_SRC_FILES := /dev/null
include $(BUILD_PREBUILT)
