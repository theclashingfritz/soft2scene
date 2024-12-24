#pragma once

#include "Types.h"

#include <assert.h>
#include <stdint.h>

#include <istream>
#include <ostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>

class BinaryFile;
class CompressedBinaryFile;

class BinaryFile {
    friend class CompressedBinaryFile;

    public:
        BinaryFile();
        BinaryFile(const char *fname);
        virtual ~BinaryFile();

		template <typename T>
		std::ostream &write(const T &data) {
			return filestream.write((char *)&data, sizeof(T));
		}

		template <typename T>
		std::ostream &write(const T *&data) {
			assert(data != nullptr);
			return filestream.write((char *)data, sizeof(T));
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

        // We need some explict write functions for some things.
        // This includes integers because they can implictly cast to one another.

        virtual std::ostream &write_int16(const int16_t &value) {
            return write(value);
        }

        virtual std::ostream &write_uint16(const uint16_t &value) {
            return write(value);
        }

        virtual std::ostream &write_int32(const int32_t &value) {
            return write(value);
        }

        virtual std::ostream &write_uint32(const uint32_t &value) {
            return write(value);
        }

        virtual std::ostream &write_int64(const int64_t &value) {
            return write(value);
        }

        virtual std::ostream &write_uint64(const uint64_t &value) {
            return write(value);
        }

    protected:
        // Stream we use for working on the data.
        std::fstream filestream;

    private:
        // Filepath used to opening our file stream.
        std::string filepath;
};

class CompressedBinaryFile : public BinaryFile {
    public:
        CompressedBinaryFile();
        CompressedBinaryFile(const char *fname);
        virtual ~CompressedBinaryFile();
        
        // For these in particular, Compress the data.
        // It can save some space, But we don't force compression on principle.
        // In future it should be a toggable option.

		std::ostream &write(const uint64_t &value) {
            uint64_t c = value & 0xFFFFFFFFFFFFFFFF;
            
            if (c == (uint64_t)-1) {
                char max_value_bytes[] = { '\xFF', '\xFF', '\xFF', '\xFF', '\x0F' };
                return filestream.write(max_value_bytes, sizeof(max_value_bytes));
            } else if (c == 0) {
                uint8_t s = 0;
                return filestream.write((char *)&s, sizeof(uint8_t));
            }
            
            while (true) {
                uint8_t b = (uint8_t)(c & 0x7fL);
                c >>= 7L;
                if (c > 0L) b |= 128L;
                std::ostream &stream = filestream.write((char *)&b, sizeof(uint8_t));
                if (c == 0) return stream;
            }
		}

        std::ostream &write(const uint32_t &value) {
            return write((uint64_t)(value & 0xFFFFFFFF));
        }

        std::ostream &write(const float &value) {
            FourByteUnion cval;
            cval.fval = value;

            return write(cval.uval);
        }

        std::ostream &write(const double &value) {
            EightByteUnion cval;
            cval.fval = value;

            return write(cval.uval);
        }

        std::ostream& write(const char *str) {
            assert(str != nullptr);
            return filestream.write(str, strlen(str));
        }

        std::ostream& write(char *const &str) {
            assert(str != nullptr);
            return filestream.write(str, strlen(str));
        }

        std::ostream& write(const std::string &str) {
            return filestream.write(str.c_str(), str.size());
        }

        std::ostream &write(const Vector3f &value) {
            write(value.x);
            write(value.y);
            return write(value.z);
        }

        std::ostream& write(const Vector3d &value) {
            write(value.x);
            write(value.y);
            return write(value.z);
        }

        std::ostream& write(const Vector4f &value) {
            write(value.x);
            write(value.y);
            write(value.z);
            return write(value.w);
        }

        std::ostream &write(const Vector4d &value) {
            write(value.x);
            write(value.y);
            write(value.z);
            return write(value.w);
        }

        std::ostream &write(const Matrix4f &value) {
            std::ostream *stream = nullptr;

            for (uint8_t i = 0; i < 4; i++) {
                for (uint8_t j = 0; j < 4; j++) {
                    stream = &write(value.mat[i][j]);
                }
            }
            return *stream;
        }

        std::ostream &write(const Matrix4d &value) {
            std::ostream *stream = nullptr;

            for (uint8_t i = 0; i < 4; i++) {
                for (uint8_t j = 0; j < 4; j++) {
                    stream = &write(value.mat[i][j]);
                }
            }
            return *stream;
        }

        virtual std::ostream &write_uint32(const uint32_t &value) {
            return write(value);
        }

        virtual std::ostream &write_uint64(const uint64_t &value) {
            return write(value);
        }

        void compress_file();
};