// RUN: "%{ocl_tester}" compile --device %{device_id} < "%s"

typedef struct {
    __local int* _wcl_preserved_local_pointer_l3;
    int _wcl_removed_local_array_f3[1];
    uint _wcl_preserved_local_array_l3[1];    
} _WclLocals;

typedef struct {
    int removed_constant_1;
} _WclConstants;

typedef struct {
    __global int *remove_variables__result_min;
    __global int *remove_variables__result_max;
} _WclGlobalLimits;

typedef struct {
    _WclGlobalLimits gl;
    __local uint *ln;
} _WclProgramAllocations;

__constant _WclConstants _wcl_constant_allocations = { 0 };

__kernel void remove_variables(__global int *result, ulong _wcl_result_size) {
    __local _WclLocals _wcl_locals;
    __local uint _wcl_local_null[1];
    _WclProgramAllocations _wcl_allocations_allocation = {
        { &result[0], &result[_wcl_result_size] },
        _wcl_local_null
    };
    _WclProgramAllocations *_wcl_allocs = &_wcl_allocations_allocation;
    (*(_wcl_locals._wcl_preserved_local_array_l3 >= _wcl_allocs->ln ? _wcl_locals._wcl_preserved_local_array_l3 : _wcl_allocs->ln)) = 0;
}
