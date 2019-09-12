# include <android/hardware/zengjf/1.0/IZengjf.h>

# include <hidl/Status.h>

# include <hidl/LegacySupport.h>

# include <utils/misc.h>

# include <hidl/HidlSupport.h>

# include <stdio.h>

using android::hardware::zengjf::V1_0::IZengjf;
using android::sp;
using android::hardware::hidl_string;

int main()
{
    int ret;

    android::sp<IZengjf> service = IZengjf::getService();
    if(service == nullptr) {
        printf("Failed to get service\n");
        return -1;
    }

    service->helloWorld("zengjf", [&](hidl_string result) {
                printf("%s\n", result.c_str());
        });

    return 0;
}
