// RUN: "%{ocl_tester}" compile --device %{device_id} < "%s"

#define _WCL_ADDRESS_SPACE_private_MIN (((8 + (CHAR_BIT - 1)) / CHAR_BIT))
#define _WCL_ADDRESS_SPACE_private_ALIGNMENT (8/CHAR_BIT)
#define _WCL_ADDRESS_SPACE_global_MIN (((32 + (CHAR_BIT - 1)) / CHAR_BIT))
#define _WCL_ADDRESS_SPACE_global_ALIGNMENT (32/CHAR_BIT)
#define _WCL_ADDRESS_SPACE_local_MIN (((32 + (CHAR_BIT - 1)) / CHAR_BIT))
#define _WCL_ADDRESS_SPACE_local_ALIGNMENT (32/CHAR_BIT)
#define _WCL_ADDRESS_SPACE_constant_MIN (((32 + (CHAR_BIT - 1)) / CHAR_BIT))
#define _WCL_ADDRESS_SPACE_constant_ALIGNMENT (32/CHAR_BIT)

typedef struct {
    int _wcl_removed_local_1;
    __local int *_wcl_removed_local_pointer_1;
    int _wcl_removed_local_array_1[1];
    int _wcl_preserved_local_f2;
    int _wcl_removed_local_l2;
    __local int *_wcl_preserved_local_pointer_f2;
    __local int *_wcl_removed_local_pointer_l2;
    int _wcl_preserved_local_array_f2[1];
    int _wcl_removed_local_array_l2[1];
    int _wcl_removed_local_f3;
    int _wcl_preserved_local_l3;
    __local int *_wcl_removed_local_pointer_f3;
    __local int *_wcl_preserved_local_pointer_l3;
    int _wcl_removed_local_array_f3[1];
    int _wcl_preserved_local_array_l3[1];
    int _wcl_preserved_local_f4;
    int _wcl_removed_local_m4;
    int _wcl_preserved_local_l4;
    __local int *_wcl_preserved_local_pointer_f4;
    __local int *_wcl_removed_local_pointer_m4;
    __local int *_wcl_preserved_local_pointer_l4;
    int _wcl_preserved_local_array_f4[1];
    int _wcl_removed_local_array_m4[1];
    int _wcl_preserved_local_array_l4[1];
    int _wcl_removed_local_f5;
    int _wcl_preserved_local_m5;
    int _wcl_removed_local_l5;
    __local int *_wcl_removed_local_pointer_f5;
    __local int *_wcl_preserved_local_pointer_m5;
    __local int *_wcl_removed_local_pointer_l5;
    int _wcl_removed_local_array_f5[1];
    int _wcl_preserved_local_array_m5[1];
    int _wcl_removed_local_array_l5[1];
    int _wcl_removed_local_f6;
    int _wcl_removed_local_m6;
    int _wcl_removed_local_l6;
    __local int *_wcl_removed_local_pointer_f6;
    __local int *_wcl_removed_local_pointer_m6;
    __local int *_wcl_removed_local_pointer_l6;
    int _wcl_removed_local_array_f6[1];
    int _wcl_removed_local_array_m6[1];
    int _wcl_removed_local_array_l6[1];
} __attribute__ ((aligned (_WCL_ADDRESS_SPACE_local_ALIGNMENT))) _WclLocals;

typedef struct {
    int removed_constant_1;
    __constant int *removed_constant_pointer_1;
    int removed_constant_array_1[1];
    int preserved_constant_f2;
    int removed_constant_l2;
    __constant int *preserved_constant_pointer_f2;
    __constant int *removed_constant_pointer_l2;
    int preserved_constant_array_f2[1];
    int removed_constant_array_l2[1];
    int preserved_constant_f3;
    int removed_constant_l3;
    __constant int *preserved_constant_pointer_f3;
    __constant int *removed_constant_pointer_l3;
    int preserved_constant_array_f3[1];
    int removed_constant_array_l3[1];
    int removed_constant_f4;
    int preserved_constant_l4;
    __constant int *removed_constant_pointer_f4;
    __constant int *preserved_constant_pointer_l4;
    int removed_constant_array_f4[1];
    int preserved_constant_array_l4[1];
    int removed_constant_f5;
    int preserved_constant_l5;
    __constant int *removed_constant_pointer_f5;
    __constant int *preserved_constant_pointer_l5;
    int removed_constant_array_f5[1];
    int preserved_constant_array_l5[1];
    int preserved_constant_f6;
    int removed_constant_m6;
    int preserved_constant_l6;
    __constant int *preserved_constant_pointer_f6;
    __constant int *removed_constant_pointer_m6;
    __constant int *preserved_constant_pointer_l6;
    int preserved_constant_array_f6[1];
    int removed_constant_array_m6[1];
    int preserved_constant_array_l6[1];
    int preserved_constant_f7;
    int removed_constant_m7;
    int preserved_constant_l7;
    __constant int *preserved_constant_pointer_f7;
    __constant int *removed_constant_pointer_m7;
    __constant int *preserved_constant_pointer_l7;
    int preserved_constant_array_f7[1];
    int removed_constant_array_m7[1];
    int preserved_constant_array_l7[1];
    int removed_constant_f8;
    int preserved_constant_m8;
    int removed_constant_l8;
    __constant int *removed_constant_pointer_f8;
    __constant int *preserved_constant_pointer_m8;
    __constant int *removed_constant_pointer_l8;
    int removed_constant_array_f8[1];
    int preserved_constant_array_m8[1];
    int removed_constant_array_l8[1];
    int removed_constant_f9;
    int preserved_constant_m9;
    int removed_constant_l9;
    __constant int *removed_constant_pointer_f9;
    __constant int *preserved_constant_pointer_m9;
    __constant int *removed_constant_pointer_l9;
    int removed_constant_array_f9[1];
    int preserved_constant_array_m9[1];
    int removed_constant_array_l9[1];
    int removed_constant_f10;
    int removed_constant_m10;
    int removed_constant_l10;
    __constant int *removed_constant_pointer_f10;
    __constant int *removed_constant_pointer_m10;
    __constant int *removed_constant_pointer_l10;
    int removed_constant_array_f10[1];
    int removed_constant_array_m10[1];
    int removed_constant_array_l10[1];
    int removed_constant_f11;
    int removed_constant_m11;
    int removed_constant_l11;
    __constant int *removed_constant_pointer_f11;
    __constant int *removed_constant_pointer_m11;
    __constant int *removed_constant_pointer_l11;
    int removed_constant_array_f11[1];
    int removed_constant_array_m11[1];
    int removed_constant_array_l11[1];
} __attribute__ ((aligned (_WCL_ADDRESS_SPACE_constant_ALIGNMENT))) _WclConstants;

typedef struct {
    __global int *remove_variables__result_min;
    __global int *remove_variables__result_max;
} _WclGlobalLimits;

typedef struct {
    __constant _WclConstants * _wcl_constant_allocations_min;
    __constant _WclConstants * _wcl_constant_allocations_max;
} _WclConstantLimits;

typedef struct {
    __local _WclLocals * _wcl_locals_min;
    __local _WclLocals * _wcl_locals_max;
} _WclLocalLimits;

typedef struct {
    _WclGlobalLimits gl;
    __global uint *gn;
    _WclConstantLimits cl;
    __constant uint *cn;
    _WclLocalLimits ll;
    __local uint *ln;
} _WclProgramAllocations;

__constant _WclConstants _wcl_constant_allocations = { 1, &_wcl_constant_allocations.removed_constant_1, { 1 }, 2, 2, &_wcl_constant_allocations.removed_constant_l2, &_wcl_constant_allocations.removed_constant_l2, { 2 }, { 2 }, 3, 3, &_wcl_constant_allocations.removed_constant_l3, &_wcl_constant_allocations.removed_constant_l3, { 3 }, { 3 }, 4, 4, &_wcl_constant_allocations.removed_constant_f4, &_wcl_constant_allocations.removed_constant_f4, { 4 }, { 4 }, 5, 5, &_wcl_constant_allocations.removed_constant_f5, &_wcl_constant_allocations.removed_constant_f5, { 5 }, { 5 }, 6, 6, 6, &_wcl_constant_allocations.removed_constant_m6, &_wcl_constant_allocations.removed_constant_m6, &_wcl_constant_allocations.removed_constant_m6, { 6 }, { 6 }, { 6 }, 7, 7, 7, &_wcl_constant_allocations.removed_constant_m7, &_wcl_constant_allocations.removed_constant_m7, &_wcl_constant_allocations.removed_constant_m7, { 7 }, { 7 }, { 7 }, 8, 8, 8, &_wcl_constant_allocations.removed_constant_f8, &_wcl_constant_allocations.removed_constant_f8, &_wcl_constant_allocations.removed_constant_f8, { 8 }, { 8 }, { 8 }, 9, 9, 9, &_wcl_constant_allocations.removed_constant_f9, &_wcl_constant_allocations.removed_constant_f9, &_wcl_constant_allocations.removed_constant_l9, { 9 }, { 9 }, { 9 }, 10, 10, 10, &_wcl_constant_allocations.removed_constant_m10, &_wcl_constant_allocations.removed_constant_m10, &_wcl_constant_allocations.removed_constant_m10, { 10 }, { 10 }, { 10 }, 11, 11, 11, &_wcl_constant_allocations.removed_constant_f11, &_wcl_constant_allocations.removed_constant_m11, &_wcl_constant_allocations.removed_constant_l11, { 11 }, { 11 }, { 11 } };

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

// be careful to edit this, this has been carefully tuned to work on every OpenCL driver
// e.g. % item_count was added to start[(items_offset+i)] = _WCL_FILLCHAR;
// to prevent compiler crash on Apple GeForce 640M
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
        start[(items_offset+i) % item_count] = _WCL_FILLCHAR;           \
    }                                                                   \
} while (0)                                                             \

#endif // cl_khr_initialize_memory

constant int hd4k_workaround_constant = 0;

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

bool _wcl_addr_check_local_1__u_ulocal__int__Ptr(__local int *addr, unsigned size, __local int * min0, __local int * max0)
{
      return 0
        || ( ((addr) >= (min0)) && ((addr + size - 1) <= _WCL_LAST(__local int *, max0)) );
}
__local int *_wcl_addr_clamp_local_1__u_ulocal__int__Ptr(__local int *addr, unsigned size, __local int * min0, __local int * max0, __local int * asnull)
{
     return _wcl_addr_check_local_1__u_ulocal__int__Ptr(addr, size, min0, max0) ? addr : asnull;
}

bool _wcl_addr_check_constant_1__u_uconstant__int__Ptr(__constant int *addr, unsigned size, __constant int * min0, __constant int * max0)
{
      return 0
        || ( ((addr) >= (min0)) && ((addr + size - 1) <= _WCL_LAST(__constant int *, max0)) );
}
__constant int *_wcl_addr_clamp_constant_1__u_uconstant__int__Ptr(__constant int *addr, unsigned size, __constant int * min0, __constant int * max0, __constant int * asnull)
{
     return _wcl_addr_check_constant_1__u_uconstant__int__Ptr(addr, size, min0, max0) ? addr : asnull;
}

__kernel void remove_variables(
    // CHECK: __global int *result, ulong _wcl_result_size)
    __global int *result, ulong _wcl_result_size)
{

    __local _WclLocals _wcl_locals;
local int hd4k_workaround_local;
hd4k_workaround_local = hd4k_workaround_constant;
    __local uint _wcl_local_null[_WCL_ADDRESS_SPACE_local_MIN];

    _WclProgramAllocations _wcl_allocations_allocation = {
        { &result[0], &result[_wcl_result_size] },
        0,
        { &(&_wcl_constant_allocations)[0], &(&_wcl_constant_allocations)[1] },
        _wcl_constant_null,
        { &(&_wcl_locals)[0], &(&_wcl_locals)[1] },
        _wcl_local_null
    };
    _WclProgramAllocations *_wcl_allocs = &_wcl_allocations_allocation;
    _wcl_allocs->gn = _WCL_SET_NULL(__global uint*, _WCL_ADDRESS_SPACE_global_MIN,_wcl_allocs->gl.remove_variables__result_min, _wcl_allocs->gl.remove_variables__result_max, (__global uint*)0);
    if (_wcl_allocs->gn == (__global uint*)0) return; // not enough space to meet the minimum access. Would be great if we could give info about the problem for the user. 

    // => Local memory zeroing.
    _WCL_LOCAL_RANGE_INIT(_wcl_allocs->ll._wcl_locals_min, _wcl_allocs->ll._wcl_locals_max);
    _WCL_LOCAL_RANGE_INIT(_wcl_local_null, _wcl_local_null + _WCL_ADDRESS_SPACE_local_MIN);
    barrier(CLK_LOCAL_MEM_FENCE);
    // <= Local memory zeroing.

    __local int removed_local_f3, preserved_local_l3;
    _wcl_locals._wcl_removed_local_f3 = 3;
    _wcl_locals._wcl_preserved_local_l3 = _wcl_locals._wcl_removed_local_f3;
    __local int * __local removed_local_pointer_f3, * __local preserved_local_pointer_l3;
    _wcl_locals._wcl_removed_local_pointer_f3 = &_wcl_locals._wcl_removed_local_f3;
    _wcl_locals._wcl_preserved_local_pointer_l3 = _wcl_locals._wcl_removed_local_pointer_f3;
    __local int removed_local_array_f3[1], preserved_local_array_l3[1];
    (*(_wcl_addr_clamp_local_1__u_ulocal__int__Ptr((_wcl_locals._wcl_removed_local_array_f3)+(0), 1, (__local int *)_wcl_allocs->ll._wcl_locals_min, (__local int *)_wcl_allocs->ll._wcl_locals_max, (__local int *)_wcl_allocs->ln))) = _wcl_locals._wcl_removed_local_f3;
    (*(_wcl_addr_clamp_local_1__u_ulocal__int__Ptr((_wcl_locals._wcl_preserved_local_array_l3)+(0), 1, (__local int *)_wcl_allocs->ll._wcl_locals_min, (__local int *)_wcl_allocs->ll._wcl_locals_max, (__local int *)_wcl_allocs->ln))) = (*(_wcl_addr_clamp_local_1__u_ulocal__int__Ptr((_wcl_locals._wcl_removed_local_array_f3)+(0), 1, (__local int *)_wcl_allocs->ll._wcl_locals_min, (__local int *)_wcl_allocs->ll._wcl_locals_max, (__local int *)_wcl_allocs->ln)));

 
}
