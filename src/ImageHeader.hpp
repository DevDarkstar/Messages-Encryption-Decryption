#pragma once

#include <string>
#include <vector>
#include <span>
#include <optional>

namespace solitaire {

	struct HeaderData {
		size_t messageLength;
		unsigned seed;
		unsigned stride;
		static constexpr size_t HEADER_SIZE = 11; // 3 bytes for message length + 4 bytes for seed + 4 bytes for stride
	};

	class ImageHeader {
	public:
		static std::vector<unsigned char> createHeader(const HeaderData& data);
		static std::optional<HeaderData> getHeaderData(std::span<const unsigned char> rawHeader, size_t imageSize);
	};
}
