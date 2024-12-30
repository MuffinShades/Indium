#include "nal.hpp"
#include "msutil.hpp"

NAL_block extract_nal_from_stream(BitStream* stream) {
	if (!stream) {
		std::cout << "Error invalid stream! [function: extract_nal_from_stream]" << std::endl;
		return {};
	}

	NAL_block block;

	block.forbidden_zero_bit = stream->readBit();
	block.nal_ref_idc = stream->readNBits(2);
	block.nal_unit_type = stream->readNBits(5);

	std::cout << "--Basic NAL Info--" << std::endl;
	std::cout << "Forbidden Zero Bit: " << (u32)block.forbidden_zero_bit << std::endl;
	std::cout << "Reference IDC: " << (u32)block.nal_ref_idc << std::endl;
	std::cout << "Unity Type: " << (u32)block.nal_unit_type << std::endl;
	std::cout << "------------------" << std::endl;

	//special stuff for nal blocks 14, 20, and 21
	if (block.nal_unit_type == 14 || block.nal_unit_type == 20 || block.nal_unit_type == 21) {
		if (block.nal_unit_type != 21)
			block.svc_ext = stream->readBit();
		else
			block.avc_3d_ext = stream->readBit();

		if (block.svc_ext) {
			//read svc extension (Annex G)
			std::cout << "svc extension" << std::endl;
			stream->skipBytes(3); //extension is 3 bytes
		}
		else if (block.avc_3d_ext) {
			//3d avc extension (Annex J)
			std::cout << "3d extension" << std::endl;
			stream->skipBytes(2);
		}
		else {
			//mvc extension (Annex H)
			std::cout << "mvc extension" << std::endl;
			stream->skipBytes(3);
		}
	}

	//read
}