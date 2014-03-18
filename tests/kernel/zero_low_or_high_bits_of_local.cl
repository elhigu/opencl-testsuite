// RUN: %{ocl_tester} compile --device %{device_id} < %s

kernel void zero_low_or_high_bits_of_local(void)
{
    local long a;
    local long b;
    local int* c = (local int*)&a;
    if (&a > &b) c++;
    *c = 0;
}
