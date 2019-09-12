#include "Zengjf.h"

namespace android {
namespace hardware {
namespace zengjf {
namespace V1_0 {
namespace implementation {

// Methods from ::android::hardware::zengjf::V1_0::IZengjf follow.
Return<void> Zengjf::helloWorld(const hidl_string& name, helloWorld_cb _hidl_cb) {
    // TODO implement
    char buf[100];
    ::memset(buf, 0x00, 100);
    ::snprintf(buf, 100, "Hello World, %s", name.c_str());
    hidl_string result(buf);

    _hidl_cb(result);
    return Void();
}


// Methods from ::android::hidl::base::V1_0::IBase follow.

IZengjf* HIDL_FETCH_IZengjf(const char* /* name */) {
    return new Zengjf();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace zengjf
}  // namespace hardware
}  // namespace android
