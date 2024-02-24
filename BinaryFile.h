#pragma once

#include "Types.h"

#include <assert.h>
#include <stdint.h>

#include <ostream>
#include <fstream>
#include <cstring>
#include <string>


class BinaryFile {
    public:
        BinaryFile(const char *fname);
        ~BinaryFile();

		template <typename T>
		std::ostream &write(const T &data) {
			return filestream.write((char *)&data, sizeof(T));
		}

		template <typename T>
		std::ostream &write(const T *&data) {
			assert(data != nullptr);
			return filestream.write((char *)data, sizeof(T));
		}

		std::ostream &write(const size_t &size) {
			uint64_t size_s = size;
			return filestream.write((char *)&size_s, sizeof(uint64_t));
		}

		std::ostream &write(const char *str) {
			assert(str != nullptr);
			return filestream.write(str, strlen(str));
		}

		std::ostream &write(char *const &str) {
			assert(str != nullptr);
			return filestream.write(str, strlen(str));
		}

		std::ostream &write(const std::string &str) {
			return filestream.write(str.c_str(), str.size());
		}

    private:
        std::fstream filestream;
};