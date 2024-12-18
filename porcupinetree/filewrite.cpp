#include "filewrite.hpp"

bool FileWrite::writeToBin(std::string src, byte* dat, size_t sz) {
	//well you screwed up step one bruh
	if (src.length() <= 0 || !dat || sz <= 0) return false;

	std::ofstream ws(src, std::ios::out | std::ios::binary);

	if (!ws.good())
		return false; //breh

	ws.write(const_cast<const char*>(reinterpret_cast<char*>(dat)), sz); //write
	ws.close();
}

/**
 * 
 * 
 */
file FileWrite::readFromBin(std::string src) {
	if (src.length() <= 0) return {};

	std::ifstream is(src, std::ios::in | std::ios::binary);

	if (!is.good())
		return {};


	//get file length
	is.seekg(0, std::ios::end);
	size_t f_len = is.tellg();
	std::cout << "File Length: " << is.tellg() << std::endl;
	is.seekg(0, std::ios::beg);

	//read into buffer
	byte* buf = new byte[f_len];
	ZeroMem(buf, f_len);

	is.read(reinterpret_cast<char*>(buf), f_len);

	return {
		.len = f_len,
		.dat = buf
	};
}