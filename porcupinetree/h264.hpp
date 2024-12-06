#pragma once
#include "types.hpp"

/**
 *  
 * NAL Block
 * 
 * network analytics something block :D
 * 
 */
struct NAL_block {
	flag forbidden_zero_bit = 0;
	u32 nal_ref_idc;
	u32 nal_unit_type;
	flag svc_ext;
	flag avc_3d_ext;
	byte* rbsp = nullptr;
	byte em_prev_three_byte;
	size_t rbsp_sz;
};

