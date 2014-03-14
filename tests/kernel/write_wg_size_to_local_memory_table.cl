// RUN: %{ocl_tester} compile --device %{device_id} < %s

kernel void write_wg_size_to_local_mem(void)
{
    local uint local_table[128];
    size_t wg_items = get_local_size(0);
    for (size_t i = 0; i < wg_items; i++) {
        local_table[i] = 0;
    }
}
