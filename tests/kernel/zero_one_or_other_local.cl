// RUN: "%{ocl_tester}" compile --device %{device_id} < "%s"

kernel void zero_one_or_other(void) {
    local uint local_1[1];
    local uint local_2[1];
    *(local_1 > local_2 ? local_1 : local_2) = 0;
}
