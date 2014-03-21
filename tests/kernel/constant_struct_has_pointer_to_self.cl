// RUN: %{ocl_tester} compile --device %{device_id} < %s

typedef struct {
    int first;
    constant int* firstPtr;
} ConstantPool;

constant ConstantPool constants = { 1, &constants.firstPtr };

void nop(constant int* addr) { }

kernel void remove_variables(void)
{
   nop(constants.firstPtr);
}
