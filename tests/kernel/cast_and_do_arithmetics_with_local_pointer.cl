// RUN: %{ocl_tester} compile --device %{device_id} < %s

typedef struct {
    int a;
    int b[1];
}  LocalStruct;

local int *test(local int *addr, local int *max)
{  
      local int* last_address = max-1;
      return (addr  <= last_address) ? addr : 0;
}

kernel void casting_local_struct_and_calling(void)
{
    local LocalStruct local_struct;
    local_struct.a = 0;
    *(test(local_struct.b, &local_struct)) = local_struct.a;
}
