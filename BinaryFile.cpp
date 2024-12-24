#define _CRT_SECURE_NO_WARNINGS

#include "BinaryFile.h"

#include <iostream>
#include <string>
#include <sstream>

#include <zlib.h>

BinaryFile::BinaryFile() {

}

BinaryFile::BinaryFile(const char *fname) : filepath(fname) {
	filestream.open(filepath, std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
	if (!filestream.is_open()) {
		std::cerr << "File Error: " << strerror(errno) << std::endl;
	}
}

BinaryFile::~BinaryFile() {
	if (filestream.is_open()) {
		filestream.close();
	}
}

CompressedBinaryFile::CompressedBinaryFile() {

}

CompressedBinaryFile::CompressedBinaryFile(const char *fname) : BinaryFile(fname) {

}

CompressedBinaryFile::~CompressedBinaryFile() {
	if (filestream.is_open()) {
		compress_file();
		filestream.close();
	}
}


void CompressedBinaryFile::compress_file() {
	filestream.seekg(0, std::ios::end);
	size_t length = filestream.tellg();
	filestream.seekg(0, std::ios::beg);

	char *inbuffer = new char[length + 1];

	// Read file
	filestream.read(inbuffer, length);
	filestream.close();

	// Reopen the file.
	filestream.open(filepath, std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);

    const size_t BUFSIZE = 128 * 1024;
    uint8_t temp_buffer[BUFSIZE];

    size_t buffer_pos = 0;
    size_t buffer_size = BUFSIZE + 1;
    char *buffer = (char *)malloc(sizeof(char) * buffer_size);
    memset(buffer, 0, buffer_size);

    // Compress our file data.
    {
        z_stream strm;
        strm.zalloc = 0;
        strm.zfree = 0;
        strm.next_in = (Bytef *)inbuffer;
        strm.avail_in = length;
        strm.next_out = temp_buffer;
        strm.avail_out = BUFSIZE;

        deflateInit(&strm, Z_BEST_COMPRESSION);

        while (strm.avail_in != 0) {
            int res = deflate(&strm, Z_NO_FLUSH);
            assert(res == Z_OK);
            if (strm.avail_out == 0) {
                buffer_size += BUFSIZE;
                buffer = (char *)realloc(buffer, sizeof(char) * buffer_size);
                memcpy(buffer + buffer_pos, temp_buffer, BUFSIZE);
                buffer_pos += BUFSIZE;

                strm.next_out = temp_buffer;
                strm.avail_out = BUFSIZE;
            }
        }

        int deflate_res = Z_OK;
        while (deflate_res == Z_OK) {
            if (strm.avail_out == 0) {
                buffer_size += BUFSIZE;
                buffer = (char *)realloc(buffer, sizeof(char) * buffer_size);
                memcpy(buffer + buffer_pos, temp_buffer, BUFSIZE);
                buffer_pos += BUFSIZE;

                strm.next_out = temp_buffer;
                strm.avail_out = BUFSIZE;
            }
            deflate_res = deflate(&strm, Z_FINISH);
        }
        assert(deflate_res == Z_STREAM_END);

        if (strm.avail_out > 0) {
            size_t write_size = BUFSIZE - strm.avail_out;
            if (buffer_pos + write_size >= buffer_size) {
                buffer_size += write_size;
                buffer = (char *)realloc(buffer, sizeof(char) * buffer_size);
            }
            memcpy(buffer + buffer_pos, temp_buffer, write_size);
            buffer_pos += write_size;
            buffer[buffer_pos] = 0;
        }

        // Reallocate to the EXACT size we need and want.
        buffer_size = buffer_pos;
        buffer = (char *)realloc(buffer, sizeof(char) * buffer_size);

        deflateEnd(&strm);
    }

    // Write the compressed buffer to our file.
    filestream.write(buffer, buffer_size);

    // Free our buffer!
    free(buffer);
}