// RUN: "%{ocl_tester}" compile --device %{device_id} < "%s"

typedef struct {
    local int* min;
    local int* max;
} Inner;

typedef struct {
    Inner ll;
} Outer;

local int* foo(local int* addr)
{
   return (((int)addr)&0x1) ? addr : addr+1;
}

kernel void nested_struct_with_local_memory_pointers(void)
{
    local int limits;
    Outer a = { { &(&limits)[0], &(&limits)[1] } };
    Outer *a_ptr = &a;
    *foo(a_ptr->ll.max) = 1;
}