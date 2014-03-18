// RUN: %{ocl_tester} compile --device %{device_id} < %s

constant uint constant_var = 0;

kernel void test(void)
{
    local uint local_var;
    local_var = 0;
}
