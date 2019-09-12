# define LOG_TAG "android.hardware.zengjf@1.0-service"

# include <android/hardware/zengjf/1.0/IZengjf.h>

# include <hidl/LegacySupport.h>

using android::hardware::zengjf::V1_0::IZengjf;
using android::hardware::defaultPassthroughServiceImplementation;

int main() {
        return defaultPassthroughServiceImplementation<IZengjf>();
}
