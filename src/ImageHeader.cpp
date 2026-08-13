#include "ImageHeader.hpp"
#include <stdexcept>

using namespace solitaire;

namespace {
    std::vector<unsigned char> convertNumberAsString(size_t value, size_t numberOfBytes) {
        std::vector<unsigned char> numberAsString(numberOfBytes);

        for (int i = 0; i < numberOfBytes; ++i) {
            numberAsString[i] = static_cast<unsigned char>(value & 255);
            value >>= 8;
        }
        return numberAsString;
    }

    std::optional<size_t> convertStringAsNumber(std::span<const unsigned char> s, size_t numberOfCharacters) {
        if (s.size() != numberOfCharacters)
            return std::nullopt;
        size_t result = 0;
        for (size_t i = 0; i < numberOfCharacters; ++i) {
            result |= static_cast<size_t>(s[i]) << (8 * i);
        }
        return result;
    }
}

std::vector<unsigned char> ImageHeader::createHeader(const HeaderData& data) {  
    std::vector<unsigned char> messageLength = convertNumberAsString(data.messageLength, 3);
	std::vector<unsigned char> seed = convertNumberAsString(data.seed, 4);
	std::vector<unsigned char> stride = convertNumberAsString(data.stride, 4);

	std::vector<unsigned char> header;
	header.reserve(HeaderData::HEADER_SIZE);

	header.insert(header.end(), messageLength.begin(), messageLength.end());
	header.insert(header.end(), seed.begin(), seed.end());
	header.insert(header.end(), stride.begin(), stride.end());
	return header;
}

std::optional<HeaderData> ImageHeader::getHeaderData(std::span<const unsigned char> rawHeader, size_t imageSize) {
    if (rawHeader.size() != HeaderData::HEADER_SIZE)
        return std::nullopt;
    HeaderData headerData{};
    std::optional<size_t> messageLength = convertStringAsNumber(rawHeader.subspan(0, 3), 3);
    if (!messageLength.has_value())
        return std::nullopt;
    else
        headerData.messageLength = *messageLength;
	std::optional<size_t> seed = convertStringAsNumber(rawHeader.subspan(3, 4), 4);
    if (!seed.has_value())
        return std::nullopt;
    else
        headerData.seed = static_cast<unsigned>(*seed);
	std::optional<size_t> stride = convertStringAsNumber(rawHeader.subspan(7, 4), 4);
    if (!stride.has_value())
        return std::nullopt;
    else
        headerData.stride = static_cast<unsigned>(*stride);

    return headerData; 
}
