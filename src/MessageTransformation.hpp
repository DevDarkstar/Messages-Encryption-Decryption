#pragma once

#include <string>
#include <vector>
#include <span>
#include "FileManager.hpp"

namespace solitaire {

	class MessageTransformation {
	public:
		static std::optional<std::vector<unsigned char>> createImage(std::string_view message, unsigned seed, const Image& image);
		static std::optional<std::string> getMessageFromImage(std::span<const unsigned char> image, int numberOfChannels, std::string& errorMessage);
		static std::vector<unsigned char> getEncryptedMessage(std::string_view messageToEncrypt, std::span<const unsigned char> streamKey);
		static std::string getDecryptionMessage(std::span<const unsigned char> messageToDecode, std::span<const unsigned char> streamKey);
	private:
		static std::vector<unsigned char> generateRandomCharacters(size_t charactersToGenerate);
		static std::string convertUTF8ToPrintableCharacters(std::string_view message);
		static std::string convertPrintableCharactersToUTF8(std::string_view message);
		static unsigned getRandomStride(unsigned minStride, unsigned maxStride);
		static std::optional<std::string> getMessageFromGeneratedImage(std::span<const unsigned char> image, std::string& errorMessage);
		static std::optional<std::string> getMessageFromExistingImage(std::span<const unsigned char> image, int numberOfChannels, std::string& errorMessage);
	};
}
