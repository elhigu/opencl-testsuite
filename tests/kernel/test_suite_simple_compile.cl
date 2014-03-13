// RUN: %{ocl_tester} compile --device %{device_id} < %s

// WebCL Validator: validation stage.
#define _WCL_ADDRESS_SPACE_private_MIN (((8 + (CHAR_BIT - 1)) / CHAR_BIT))
#define _WCL_ADDRESS_SPACE_private_ALIGNMENT (8/CHAR_BIT)
#define _WCL_ADDRESS_SPACE_global_MIN (((32 + (CHAR_BIT - 1)) / CHAR_BIT))
#define _WCL_ADDRESS_SPACE_global_ALIGNMENT (32/CHAR_BIT)
#define _WCL_ADDRESS_SPACE_local_MIN (((128 + (CHAR_BIT - 1)) / CHAR_BIT))
#define _WCL_ADDRESS_SPACE_local_ALIGNMENT (128/CHAR_BIT)
#define _WCL_ADDRESS_SPACE_constant_MIN (((8 + (CHAR_BIT - 1)) / CHAR_BIT))
#define _WCL_ADDRESS_SPACE_constant_ALIGNMENT (8/CHAR_BIT)
typedef struct {
    int _wcl_int_variable;
    char _wcl_char_variable;
    float _wcl_float_variable;
    float4 _wcl_vector_variable;
} __attribute__ ((aligned (_WCL_ADDRESS_SPACE_local_ALIGNMENT))) _WclLocals;

typedef struct {
    __global int *zero_local_mem__result_min;
    __global int *zero_local_mem__result_max;
} _WclGlobalLimits;

typedef struct {
    __local _WclLocals * _wcl_locals_min;
    __local _WclLocals * _wcl_locals_max;
    __local int *zero_local_mem__int_array_min;
    __local int *zero_local_mem__int_array_max;
    __local char *zero_local_mem__char_array_min;
    __local char *zero_local_mem__char_array_max;
    __local float *zero_local_mem__float_array_min;
    __local float *zero_local_mem__float_array_max;
    __local float4 *zero_local_mem__vector_array_min;
    __local float4 *zero_local_mem__vector_array_max;
} _WclLocalLimits;

typedef struct {
    _WclGlobalLimits gl;
    __global uint *gn;
    _WclLocalLimits ll;
    __local uint *ln;
} _WclProgramAllocations;

__constant uint _wcl_constant_null[_WCL_ADDRESS_SPACE_constant_MIN] = { 0 };

// => General code that doesn't depend on input.

#define _WCL_MEMCPY(dst, src) for(ulong i = 0; i < sizeof((src))/sizeof((src)[0]); i++) { (dst)[i] = (src)[i]; }

#define _WCL_LAST(type, ptr) (((type)(ptr)) - 1)
#define _WCL_FILLCHAR ((uchar)0xCC)

// POCL crashes at run time if the parameters are local character
// pointers.
typedef uint _WclInitType;

// NOTE: this expects that null pointer is type of uint*
#define _WCL_SET_NULL(type, req_bytes, min, max, null) ( ((((type)max)-((type)min))*sizeof(uint) >= req_bytes) ? ((type)min) : (null) )

#ifdef cl_khr_initialize_memory
#pragma OPENCL EXTENSION cl_khr_initialize_memory : enable
#define _WCL_LOCAL_RANGE_INIT(begin, end)
#else

#define _WCL_LOCAL_RANGE_INIT(begin, end) do {               \
    __local uchar *start = (__local uchar *)begin;           \
    __local uchar *stop = (__local uchar *)end;              \
    const size_t z_items = get_local_size(2);                \
    const size_t yz_items = get_local_size(1) * z_items;     \
    const size_t xyz_items = get_local_size(0) * yz_items;   \
    const size_t item_index =                                \
        (get_local_id(0) * yz_items) +                       \
        (get_local_id(1) * z_items) +                        \
        get_local_id(2);                                     \
    size_t item_count = stop - start;                        \
    size_t items_per_kernel = item_count / xyz_items;        \
    size_t items_offset = items_per_kernel * item_index;     \
    size_t reminders = item_count % xyz_items;               \
    if (item_index < reminders) {                            \
        start[xyz_items*items_per_kernel + item_index] = _WCL_FILLCHAR; \
    }                                                                   \
    for (size_t i = 0; i < items_per_kernel; i++) {                     \
        start[items_offset+i] = _WCL_FILLCHAR;                          \
    }                                                                   \
} while (0)                                                             \

#endif // cl_khr_initialize_memory

// <= General code that doesn't depend on input.

bool _wcl_addr_check_global_1__u_uglobal__int__Ptr(__global int *addr, unsigned size, __global int * min0, __global int * max0)
{
      return 0
        || ( ((addr) >= (min0)) && ((addr + size - 1) <= _WCL_LAST(__global int *, max0)) );
}
__global int *_wcl_addr_clamp_global_1__u_uglobal__int__Ptr(__global int *addr, unsigned size, __global int * min0, __global int * max0, __global int * asnull)
{
     return _wcl_addr_check_global_1__u_uglobal__int__Ptr(addr, size, min0, max0) ? addr : asnull;
}

bool _wcl_addr_check_local_5__u_ulocal__char__Ptr(__local char *addr, unsigned size, __local char * min0, __local char * max0, __local char * min1, __local char * max1, __local char * min2, __local char * max2, __local char * min3, __local char * max3, __local char * min4, __local char * max4)
{
      return 0
        || ( ((addr) >= (min0)) && ((addr + size - 1) <= _WCL_LAST(__local char *, max0)) )
        || ( ((addr) >= (min1)) && ((addr + size - 1) <= _WCL_LAST(__local char *, max1)) )
        || ( ((addr) >= (min2)) && ((addr + size - 1) <= _WCL_LAST(__local char *, max2)) )
        || ( ((addr) >= (min3)) && ((addr + size - 1) <= _WCL_LAST(__local char *, max3)) )
        || ( ((addr) >= (min4)) && ((addr + size - 1) <= _WCL_LAST(__local char *, max4)) );
}
__local char *_wcl_addr_clamp_local_5__u_ulocal__char__Ptr(__local char *addr, unsigned size, __local char * min0, __local char * max0, __local char * min1, __local char * max1, __local char * min2, __local char * max2, __local char * min3, __local char * max3, __local char * min4, __local char * max4, __local char * asnull)
{
     return _wcl_addr_check_local_5__u_ulocal__char__Ptr(addr, size, min0, max0, min1, max1, min2, max2, min3, max3, min4, max4) ? addr : asnull;
}

bool _wcl_addr_check_local_5__u_ulocal__float__Ptr(__local float *addr, unsigned size, __local float * min0, __local float * max0, __local float * min1, __local float * max1, __local float * min2, __local float * max2, __local float * min3, __local float * max3, __local float * min4, __local float * max4)
{
      return 0
        || ( ((addr) >= (min0)) && ((addr + size - 1) <= _WCL_LAST(__local float *, max0)) )
        || ( ((addr) >= (min1)) && ((addr + size - 1) <= _WCL_LAST(__local float *, max1)) )
        || ( ((addr) >= (min2)) && ((addr + size - 1) <= _WCL_LAST(__local float *, max2)) )
        || ( ((addr) >= (min3)) && ((addr + size - 1) <= _WCL_LAST(__local float *, max3)) )
        || ( ((addr) >= (min4)) && ((addr + size - 1) <= _WCL_LAST(__local float *, max4)) );
}
__local float *_wcl_addr_clamp_local_5__u_ulocal__float__Ptr(__local float *addr, unsigned size, __local float * min0, __local float * max0, __local float * min1, __local float * max1, __local float * min2, __local float * max2, __local float * min3, __local float * max3, __local float * min4, __local float * max4, __local float * asnull)
{
     return _wcl_addr_check_local_5__u_ulocal__float__Ptr(addr, size, min0, max0, min1, max1, min2, max2, min3, max3, min4, max4) ? addr : asnull;
}

bool _wcl_addr_check_local_5__u_ulocal__float4__Ptr(__local float4 *addr, unsigned size, __local float4 * min0, __local float4 * max0, __local float4 * min1, __local float4 * max1, __local float4 * min2, __local float4 * max2, __local float4 * min3, __local float4 * max3, __local float4 * min4, __local float4 * max4)
{
      return 0
        || ( ((addr) >= (min0)) && ((addr + size - 1) <= _WCL_LAST(__local float4 *, max0)) )
        || ( ((addr) >= (min1)) && ((addr + size - 1) <= _WCL_LAST(__local float4 *, max1)) )
        || ( ((addr) >= (min2)) && ((addr + size - 1) <= _WCL_LAST(__local float4 *, max2)) )
        || ( ((addr) >= (min3)) && ((addr + size - 1) <= _WCL_LAST(__local float4 *, max3)) )
        || ( ((addr) >= (min4)) && ((addr + size - 1) <= _WCL_LAST(__local float4 *, max4)) );
}
__local float4 *_wcl_addr_clamp_local_5__u_ulocal__float4__Ptr(__local float4 *addr, unsigned size, __local float4 * min0, __local float4 * max0, __local float4 * min1, __local float4 * max1, __local float4 * min2, __local float4 * max2, __local float4 * min3, __local float4 * max3, __local float4 * min4, __local float4 * max4, __local float4 * asnull)
{
     return _wcl_addr_check_local_5__u_ulocal__float4__Ptr(addr, size, min0, max0, min1, max1, min2, max2, min3, max3, min4, max4) ? addr : asnull;
}

bool _wcl_addr_check_local_5__u_ulocal__int__Ptr(__local int *addr, unsigned size, __local int * min0, __local int * max0, __local int * min1, __local int * max1, __local int * min2, __local int * max2, __local int * min3, __local int * max3, __local int * min4, __local int * max4)
{
      return 0
        || ( ((addr) >= (min0)) && ((addr + size - 1) <= _WCL_LAST(__local int *, max0)) )
        || ( ((addr) >= (min1)) && ((addr + size - 1) <= _WCL_LAST(__local int *, max1)) )
        || ( ((addr) >= (min2)) && ((addr + size - 1) <= _WCL_LAST(__local int *, max2)) )
        || ( ((addr) >= (min3)) && ((addr + size - 1) <= _WCL_LAST(__local int *, max3)) )
        || ( ((addr) >= (min4)) && ((addr + size - 1) <= _WCL_LAST(__local int *, max4)) );
}
__local int *_wcl_addr_clamp_local_5__u_ulocal__int__Ptr(__local int *addr, unsigned size, __local int * min0, __local int * max0, __local int * min1, __local int * max1, __local int * min2, __local int * max2, __local int * min3, __local int * max3, __local int * min4, __local int * max4, __local int * asnull)
{
     return _wcl_addr_check_local_5__u_ulocal__int__Ptr(addr, size, min0, max0, min1, max1, min2, max2, min3, max3, min4, max4) ? addr : asnull;
}

void init_local_mem(_WclProgramAllocations *_wcl_allocs, 
    __local int *int_array, int int_value,
    __local char *char_array, char char_value,
    __local float *float_array, float float_value,
    __local float4 *vector_array, float4 vector_value);

void init_local_mem(_WclProgramAllocations *_wcl_allocs, 
    __local int *int_array, int int_value,
    __local char *char_array, char char_value,
    __local float *float_array, float float_value,
    __local float4 *vector_array, float4 vector_value)
{
    (*(_wcl_addr_clamp_local_5__u_ulocal__int__Ptr((int_array)+(0), 1, (__local int *)_wcl_allocs->ll._wcl_locals_min, (__local int *)_wcl_allocs->ll._wcl_locals_max, (__local int *)_wcl_allocs->ll.zero_local_mem__int_array_min, (__local int *)_wcl_allocs->ll.zero_local_mem__int_array_max, (__local int *)_wcl_allocs->ll.zero_local_mem__char_array_min, (__local int *)_wcl_allocs->ll.zero_local_mem__char_array_max, (__local int *)_wcl_allocs->ll.zero_local_mem__float_array_min, (__local int *)_wcl_allocs->ll.zero_local_mem__float_array_max, (__local int *)_wcl_allocs->ll.zero_local_mem__vector_array_min, (__local int *)_wcl_allocs->ll.zero_local_mem__vector_array_max, (__local int *)_wcl_allocs->ln))) = int_value;
    (*(_wcl_addr_clamp_local_5__u_ulocal__char__Ptr((char_array)+(0), 1, (__local char *)_wcl_allocs->ll._wcl_locals_min, (__local char *)_wcl_allocs->ll._wcl_locals_max, (__local char *)_wcl_allocs->ll.zero_local_mem__int_array_min, (__local char *)_wcl_allocs->ll.zero_local_mem__int_array_max, (__local char *)_wcl_allocs->ll.zero_local_mem__char_array_min, (__local char *)_wcl_allocs->ll.zero_local_mem__char_array_max, (__local char *)_wcl_allocs->ll.zero_local_mem__float_array_min, (__local char *)_wcl_allocs->ll.zero_local_mem__float_array_max, (__local char *)_wcl_allocs->ll.zero_local_mem__vector_array_min, (__local char *)_wcl_allocs->ll.zero_local_mem__vector_array_max, (__local char *)_wcl_allocs->ln))) = char_value;
    (*(_wcl_addr_clamp_local_5__u_ulocal__float__Ptr((float_array)+(0), 1, (__local float *)_wcl_allocs->ll._wcl_locals_min, (__local float *)_wcl_allocs->ll._wcl_locals_max, (__local float *)_wcl_allocs->ll.zero_local_mem__int_array_min, (__local float *)_wcl_allocs->ll.zero_local_mem__int_array_max, (__local float *)_wcl_allocs->ll.zero_local_mem__char_array_min, (__local float *)_wcl_allocs->ll.zero_local_mem__char_array_max, (__local float *)_wcl_allocs->ll.zero_local_mem__float_array_min, (__local float *)_wcl_allocs->ll.zero_local_mem__float_array_max, (__local float *)_wcl_allocs->ll.zero_local_mem__vector_array_min, (__local float *)_wcl_allocs->ll.zero_local_mem__vector_array_max, (__local float *)_wcl_allocs->ln))) = float_value;
    (*(_wcl_addr_clamp_local_5__u_ulocal__float4__Ptr((vector_array)+(0), 1, (__local float4 *)_wcl_allocs->ll._wcl_locals_min, (__local float4 *)_wcl_allocs->ll._wcl_locals_max, (__local float4 *)_wcl_allocs->ll.zero_local_mem__int_array_min, (__local float4 *)_wcl_allocs->ll.zero_local_mem__int_array_max, (__local float4 *)_wcl_allocs->ll.zero_local_mem__char_array_min, (__local float4 *)_wcl_allocs->ll.zero_local_mem__char_array_max, (__local float4 *)_wcl_allocs->ll.zero_local_mem__float_array_min, (__local float4 *)_wcl_allocs->ll.zero_local_mem__float_array_max, (__local float4 *)_wcl_allocs->ll.zero_local_mem__vector_array_min, (__local float4 *)_wcl_allocs->ll.zero_local_mem__vector_array_max, (__local float4 *)_wcl_allocs->ln))) = vector_value;
}

__kernel void zero_local_mem(
    __global int *result, ulong _wcl_result_size,
    __local int *int_array, ulong _wcl_int_array_size,
    __local char *char_array, ulong _wcl_char_array_size,
    __local float *float_array, ulong _wcl_float_array_size,
    __local float4 *vector_array, ulong _wcl_vector_array_size)
{

    __local _WclLocals _wcl_locals;
    __local uint _wcl_local_null[_WCL_ADDRESS_SPACE_local_MIN];

    _WclProgramAllocations _wcl_allocations_allocation = {
        { &result[0], &result[_wcl_result_size] },
        0,
        { &(&_wcl_locals)[0], &(&_wcl_locals)[1],&int_array[0], &int_array[_wcl_int_array_size],&char_array[0], &char_array[_wcl_char_array_size],&float_array[0], &float_array[_wcl_float_array_size],&vector_array[0], &vector_array[_wcl_vector_array_size] },
        _wcl_local_null
    };
    _WclProgramAllocations *_wcl_allocs = &_wcl_allocations_allocation;
    _wcl_allocs->gn = _WCL_SET_NULL(__global uint*, _WCL_ADDRESS_SPACE_global_MIN,_wcl_allocs->gl.zero_local_mem__result_min, _wcl_allocs->gl.zero_local_mem__result_max, (__global uint*)0);
    if (_wcl_allocs->gn == (__global uint*)0) return; // not enough space to meet the minimum access. Would be great if we could give info about the problem for the user. 

    // => Local memory zeroing.
    _WCL_LOCAL_RANGE_INIT(_wcl_allocs->ll._wcl_locals_min, _wcl_allocs->ll._wcl_locals_max);
    _WCL_LOCAL_RANGE_INIT(_wcl_allocs->ll.zero_local_mem__int_array_min, _wcl_allocs->ll.zero_local_mem__int_array_max);
    _WCL_LOCAL_RANGE_INIT(_wcl_allocs->ll.zero_local_mem__char_array_min, _wcl_allocs->ll.zero_local_mem__char_array_max);
    _WCL_LOCAL_RANGE_INIT(_wcl_allocs->ll.zero_local_mem__float_array_min, _wcl_allocs->ll.zero_local_mem__float_array_max);
    _WCL_LOCAL_RANGE_INIT(_wcl_allocs->ll.zero_local_mem__vector_array_min, _wcl_allocs->ll.zero_local_mem__vector_array_max);
    _WCL_LOCAL_RANGE_INIT(_wcl_local_null, _wcl_local_null + _WCL_ADDRESS_SPACE_local_MIN);
    barrier(CLK_LOCAL_MEM_FENCE);
    // <= Local memory zeroing.

    // Each range should be zeroed.

    // CHECK: _WCL_LOCAL_RANGE_INIT(_wcl_allocs->ll._wcl_locals_min, _wcl_allocs->ll._wcl_locals_max);
    // CHECK: _WCL_LOCAL_RANGE_INIT(_wcl_allocs->ll.zero_local_mem__int_array_min, _wcl_allocs->ll.zero_local_mem__int_array_max);
    // CHECK: _WCL_LOCAL_RANGE_INIT(_wcl_allocs->ll.zero_local_mem__char_array_min, _wcl_allocs->ll.zero_local_mem__char_array_max);
    // CHECK: _WCL_LOCAL_RANGE_INIT(_wcl_allocs->ll.zero_local_mem__float_array_min, _wcl_allocs->ll.zero_local_mem__float_array_max);
    // CHECK: _WCL_LOCAL_RANGE_INIT(_wcl_allocs->ll.zero_local_mem__vector_array_min, _wcl_allocs->ll.zero_local_mem__vector_array_max);
    // CHECK: barrier(CLK_LOCAL_MEM_FENCE);

    // Initialization should occur after zeroing.

    __local int int_variable;
    // CHECK: _wcl_locals._wcl_int_variable = 1;
    _wcl_locals._wcl_int_variable = 1;
    __local char char_variable;
    // CHECK: _wcl_locals._wcl_char_variable = 'a';
    _wcl_locals._wcl_char_variable = 'a';
    __local float float_variable;
    // CHECK: _wcl_locals._wcl_float_variable = 1.0f;
    _wcl_locals._wcl_float_variable = 1.0f;
    __local float4 vector_variable;
    // CHECK: _wcl_locals._wcl_vector_variable = ((float4)(1.0f));
    _wcl_locals._wcl_vector_variable = ((float4)(1.0f));

    const int i = get_global_id(0);

    init_local_mem(_wcl_allocs, int_array, _wcl_locals._wcl_int_variable,
                   char_array, _wcl_locals._wcl_char_variable,
                   float_array, _wcl_locals._wcl_float_variable,
                   vector_array, _wcl_locals._wcl_vector_variable);

    (*(_wcl_addr_clamp_global_1__u_uglobal__int__Ptr((result)+(i), 1, (__global int *)_wcl_allocs->gl.zero_local_mem__result_min, (__global int *)_wcl_allocs->gl.zero_local_mem__result_max, (__global int *)_wcl_allocs->gn))) =
        ((*(_wcl_addr_clamp_local_5__u_ulocal__int__Ptr((int_array)+(i), 1, (__local int *)_wcl_allocs->ll._wcl_locals_min, (__local int *)_wcl_allocs->ll._wcl_locals_max, (__local int *)_wcl_allocs->ll.zero_local_mem__int_array_min, (__local int *)_wcl_allocs->ll.zero_local_mem__int_array_max, (__local int *)_wcl_allocs->ll.zero_local_mem__char_array_min, (__local int *)_wcl_allocs->ll.zero_local_mem__char_array_max, (__local int *)_wcl_allocs->ll.zero_local_mem__float_array_min, (__local int *)_wcl_allocs->ll.zero_local_mem__float_array_max, (__local int *)_wcl_allocs->ll.zero_local_mem__vector_array_min, (__local int *)_wcl_allocs->ll.zero_local_mem__vector_array_max, (__local int *)_wcl_allocs->ln))) == _wcl_locals._wcl_int_variable) &&
        ((*(_wcl_addr_clamp_local_5__u_ulocal__char__Ptr((char_array)+(i), 1, (__local char *)_wcl_allocs->ll._wcl_locals_min, (__local char *)_wcl_allocs->ll._wcl_locals_max, (__local char *)_wcl_allocs->ll.zero_local_mem__int_array_min, (__local char *)_wcl_allocs->ll.zero_local_mem__int_array_max, (__local char *)_wcl_allocs->ll.zero_local_mem__char_array_min, (__local char *)_wcl_allocs->ll.zero_local_mem__char_array_max, (__local char *)_wcl_allocs->ll.zero_local_mem__float_array_min, (__local char *)_wcl_allocs->ll.zero_local_mem__float_array_max, (__local char *)_wcl_allocs->ll.zero_local_mem__vector_array_min, (__local char *)_wcl_allocs->ll.zero_local_mem__vector_array_max, (__local char *)_wcl_allocs->ln))) == _wcl_locals._wcl_char_variable) &&
        ((*(_wcl_addr_clamp_local_5__u_ulocal__float__Ptr((float_array)+(i), 1, (__local float *)_wcl_allocs->ll._wcl_locals_min, (__local float *)_wcl_allocs->ll._wcl_locals_max, (__local float *)_wcl_allocs->ll.zero_local_mem__int_array_min, (__local float *)_wcl_allocs->ll.zero_local_mem__int_array_max, (__local float *)_wcl_allocs->ll.zero_local_mem__char_array_min, (__local float *)_wcl_allocs->ll.zero_local_mem__char_array_max, (__local float *)_wcl_allocs->ll.zero_local_mem__float_array_min, (__local float *)_wcl_allocs->ll.zero_local_mem__float_array_max, (__local float *)_wcl_allocs->ll.zero_local_mem__vector_array_min, (__local float *)_wcl_allocs->ll.zero_local_mem__vector_array_max, (__local float *)_wcl_allocs->ln))) == _wcl_locals._wcl_float_variable) &&
        ((*(_wcl_addr_clamp_local_5__u_ulocal__float4__Ptr((vector_array)+(i), 1, (__local float4 *)_wcl_allocs->ll._wcl_locals_min, (__local float4 *)_wcl_allocs->ll._wcl_locals_max, (__local float4 *)_wcl_allocs->ll.zero_local_mem__int_array_min, (__local float4 *)_wcl_allocs->ll.zero_local_mem__int_array_max, (__local float4 *)_wcl_allocs->ll.zero_local_mem__char_array_min, (__local float4 *)_wcl_allocs->ll.zero_local_mem__char_array_max, (__local float4 *)_wcl_allocs->ll.zero_local_mem__float_array_min, (__local float4 *)_wcl_allocs->ll.zero_local_mem__float_array_max, (__local float4 *)_wcl_allocs->ll.zero_local_mem__vector_array_min, (__local float4 *)_wcl_allocs->ll.zero_local_mem__vector_array_max, (__local float4 *)_wcl_allocs->ln))).x == _wcl_locals._wcl_vector_variable.x);
}
