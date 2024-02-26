#define _CRT_SECURE_NO_WARNINGS

#include "BinaryFile.h"

#include <iostream>

BinaryFile::BinaryFile(const char *fname) {
	filestream.open(fname, std::ios::binary | std::ios::out | std::ios::trunc);
	if (!filestream.is_open()) {
		std::cerr << "File Error: " << strerror(errno) << std::endl;
	}
}

BinaryFile::~BinaryFile() {
	if (filestream.is_open()) {
		filestream.close();
	}
}

CompressedBinaryFile::CompressedBinaryFile(const char *fname) : BinaryFile(fname) {

}

CompressedBinaryFile::~CompressedBinaryFile() {
	if (filestream.is_open()) {
		filestream.close();
	}
}