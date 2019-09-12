#ifndef ANDROID_HARDWARE_ZENGJF_V1_0_ZENGJF_H
#define ANDROID_HARDWARE_ZENGJF_V1_0_ZENGJF_H

#include <android/hardware/zengjf/1.0/IZengjf.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

namespace android {
namespace hardware {
namespace zengjf {
namespace V1_0 {
namespace implementation {

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct Zengjf : public IZengjf {
    // Methods from ::android::hardware::zengjf::V1_0::IZengjf follow.
    Return<void> helloWorld(const hidl_string& name, helloWorld_cb _hidl_cb) override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.

};

// FIXME: most likely delete, this is only for passthrough implementations
extern "C" IZengjf* HIDL_FETCH_IZengjf(const char* name);

}  // namespace implementation
}  // namespace V1_0
}  // namespace zengjf
}  // namespace hardware
}  // namespace android

#endif  // ANDROID_HARDWARE_ZENGJF_V1_0_ZENGJF_H
