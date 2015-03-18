#ifdef LIBXENOVERSE_ZLIB_SUPPORT

namespace LibXenoverse {
	EMZ::EMZ(string filename) {
		File file(filename, LIBXENOVERSE_FILE_READ_BINARY);
		if (file.valid() && file.readHeader(LIBXENOVERSE_EMZ_SIGNATURE, LIBXENOVERSE_LITTLE_ENDIAN)) {
			// Read Header
			file.goToAddress(8);
			unsigned int uncompressed_size = 0;
			unsigned int compressed_data_offset = 0;
			file.readInt32E(&uncompressed_size);
			file.readInt32E(&compressed_data_offset);

			data = new unsigned char[uncompressed_size];
			data_size = uncompressed_size;
			
			// Read Compressed Data
			size_t buffer_size = file.getFileSize() - compressed_data_offset;
			unsigned char *compressed_data = new unsigned char[buffer_size];
			file.goToAddress(compressed_data_offset);
			file.read(compressed_data, buffer_size);
			file.close();

			int ret;
			z_stream zstream;
			zstream.zalloc = (alloc_func)Z_NULL;
			zstream.zfree = (free_func)Z_NULL;
			zstream.opaque = (voidpf)Z_NULL;

			int zlib_status = inflateInit2(&zstream, -15);

			if (zlib_status == Z_OK) {
				size_t current_offset = 0;
				size_t current_data_offset = 0;

				do {
					// Calculate Remaining Bytes
					zstream.avail_in = min(LIBXENOVERSE_EMZ_CHUNK, max(buffer_size - current_offset, 0));
					if (zstream.avail_in == 0) break;

					// Move in pointer to the current compressed data chunk
					zstream.next_in = compressed_data + current_offset;
					current_offset += zstream.avail_in;
					do
					{
						zstream.avail_out = LIBXENOVERSE_EMZ_CHUNK;
						zstream.next_out = data + current_data_offset;
						ret = inflate(&zstream, Z_NO_FLUSH);
						current_data_offset += LIBXENOVERSE_EMZ_CHUNK - zstream.avail_out;

						if (ret != Z_OK) {
							break;
						}
					}
					while (zstream.avail_out == 0);
				} while (ret != Z_STREAM_END);

				inflateEnd(&zstream);
			}
			else {
				return;
			}

			delete [] compressed_data;
		}
	}


	void EMZ::saveUncompressed(string filename) {
		File file(filename, LIBXENOVERSE_FILE_WRITE_BINARY);
		if (file.valid()) {
			file.write(data, data_size);
			file.close();
		}
	}

	string EMZ::detectNewExtension() {
		if ((!data) || (data_size <= 4)) {
			return ".emz.invalid";
		}

		char data_header[5]="#EMZ";
		strncpy(data_header, (char *) data, 4);

		if (!strcmp(data_header, LIBXENOVERSE_EMB_SIGNATURE)) {
			return LIBXENOVERSE_EMB_EXTENSION;
		}

		return ".emz.uncompressed";
	}
}

#endif