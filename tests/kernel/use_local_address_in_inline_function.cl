// RUN: %{ocl_tester} compile --device %{device_id} < %s

typedef struct {
    local int * min;
    local int * max;
} Limits;

inline void foo(local int* addr)
{
    addr ? 0 : 1;
}

kernel void local_address_in_inline_function(void)
{
    local int range[1];
    Limits ll = { range, range + 1 };
    foo(ll.max);

}