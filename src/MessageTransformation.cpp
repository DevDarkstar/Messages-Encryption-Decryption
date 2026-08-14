#include "MessageTransformation.hpp"
#include "SolitaireAlgorithm.hpp"
#include "ImageHeader.hpp"
#include "utf8.h"
#include <array>
#include <iostream>
#include <random>
#include <optional>

using namespace solitaire;

namespace {
    // alphabet used to encrypt messages
    //constexpr std::string_view alphabet = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\x7f";
	constexpr size_t alphabetLength = 256;
    // Reserved bytes in files or image for message length
	// Sorted array of pairs to map special characters to a representation containing only printable characters.
    constexpr std::array<const std::pair<utf8::utfchar32_t, std::string_view>, 71> utf8ToPrintableCharacters{ {
        {0x0009, R"(\ht)"},           // tabulation
        {0x000A, R"(\lf)"},           // newline
        {0x000D, R"(\cr)"},           // carriage return
        {0x0080, R"(\eu)"},           // €
        {0x0099, R"(\tm)"},           // ™
        {0x00A3, R"(\pn)"},           // £
        {0x00AB, R"(\gl)"},           // «
        {0x00BB, R"(\gr)"},           // »
        {0x00BF, R"(\`?)"},           // ¿
        {0x00C0, R"(\`A)"},           // À
        {0x00C1, R"(\'A)"},           // Á
        {0x00C2, R"(\^A)"},           // Â
        {0x00C3, R"(\~A)"},           // Ã
        {0x00C4, R"(\"A)"},           // Ä
        {0x00C5, R"(\rA)"},           // Å
        {0x00C6, R"(\AE)"},           // Æ
        {0x00C7, R"(\cC)"},           // Ç
        {0x00C8, R"(\`E)"},           // È
        {0x00C9, R"(\'E)"},           // É
        {0x00CA, R"(\^E)"},           // Ê
        {0x00CB, R"(\"E)"},           // Ë
        {0x00CC, R"(\`I)"},           // Ì
        {0x00CD, R"(\'I)"},           // Í
        {0x00CE, R"(\^I)"},           // Î
        {0x00CF, R"(\"I)"},           // Ï
        {0x00D1, R"(\~N)"},           // Ñ
        {0x00D2, R"(\`O)"},           // Ò
        {0x00D3, R"(\'O)"},           // Ó
        {0x00D4, R"(\^O)"},           // Ô
        {0x00D5, R"(\~O)"},           // Õ
        {0x00D6, R"(\"O)"},           // Ö
        {0x00D8, R"(\oO)"},           // Ø
        {0x00D9, R"(\`U)"},           // Ù
        {0x00DA, R"(\'U)"},           // Ú
        {0x00DB, R"(\^U)"},           // Û
        {0x00DC, R"(\"U)"},           // Ü
        {0x00DF, R"(\ss)"},           // ß
        {0x00E0, R"(\`a)"},           // à
        {0x00E1, R"(\'a)"},           // á
        {0x00E2, R"(\^a)"},           // â
        {0x00E3, R"(\~a)"},           // ã
        {0x00E4, R"(\"a)"},           // ä
        {0x00E5, R"(\ra)"},           // å
        {0x00E6, R"(\ae)"},           // æ
        {0x00E7, R"(\cc)"},           // ç
        {0x00E8, R"(\`e)"},           // è
        {0x00E9, R"(\'e)"},           // é
        {0x00EA, R"(\^e)"},           // ê
        {0x00EB, R"(\"e)"},           // ë
        {0x00EC, R"(\`i)"},           // ì
        {0x00ED, R"(\'i)"},           // í
        {0x00EE, R"(\^i)"},           // î
        {0x00EF, R"(\"i)"},           // ï
        {0x00F1, R"(\~n)"},           // ñ
        {0x00F2, R"(\`o)"},           // ò
        {0x00F3, R"(\'o)"},           // ó
        {0x00F4, R"(\^o)"},           // ô
        {0x00F5, R"(\~o)"},           // õ
        {0x00F6, R"(\"o)"},           // ö
        {0x00F8, R"(\oo)"},           // ø
        {0x00F9, R"(\`u)"},           // ù
        {0x00FA, R"(\'u)"},           // ú
        {0x00FB, R"(\^u)"},           // û
        {0x00FC, R"(\"u)"},           // ü
        {0x0150, R"(\HO)"},           // Ő
        {0x0151, R"(\Ho)"},           // ő
        {0x0152, R"(\OE)"},           // Œ
        {0x0153, R"(\oe)"},           // œ
        {0x0170, R"(\HU)"},           // Ű
        {0x0171, R"(\Hu)"},           // ű
        {0x2019, R"(\rq)"}            // ’
    } };

    constexpr uint32_t makePatternKey(std::string_view pattern)
    {
        return (static_cast<uint32_t>(static_cast<unsigned char>(pattern[0])) << 16) |
               (static_cast<uint32_t>(static_cast<unsigned char>(pattern[1])) << 8) |
               static_cast<uint32_t>(static_cast<unsigned char>(pattern[2]));
    }

    constexpr std::array<const std::pair<uint32_t, utf8::utfchar32_t>, 71> printableCharactersToUTF8{ {
        {makePatternKey(R"(\"A)"), 0x00C4}, // Ä
        {makePatternKey(R"(\"E)"), 0x00CB}, // Ë
        {makePatternKey(R"(\"I)"), 0x00CF}, // Ï
        {makePatternKey(R"(\"O)"), 0x00D6}, // Ö
        {makePatternKey(R"(\"U)"), 0x00DC}, // Ü
        {makePatternKey(R"(\"a)"), 0x00E4}, // ä
        {makePatternKey(R"(\"e)"), 0x00EB}, // ë
        {makePatternKey(R"(\"i)"), 0x00EF}, // ï
        {makePatternKey(R"(\"o)"), 0x00F6}, // ö
        {makePatternKey(R"(\"u)"), 0x00FC}, // ü      

        {makePatternKey(R"(\'A)"), 0x00C1}, // Á
        {makePatternKey(R"(\'E)"), 0x00C9}, // É
        {makePatternKey(R"(\'I)"), 0x00CD}, // Í
        {makePatternKey(R"(\'O)"), 0x00D3}, // Ó
        {makePatternKey(R"(\'U)"), 0x00DA}, // Ú
        {makePatternKey(R"(\'a)"), 0x00E1}, // á
        {makePatternKey(R"(\'e)"), 0x00E9}, // é
        {makePatternKey(R"(\'i)"), 0x00ED}, // í
        {makePatternKey(R"(\'o)"), 0x00F3}, // ó
        {makePatternKey(R"(\'u)"), 0x00FA}, // ú

        {makePatternKey(R"(\AE)"), 0x00C6}, // Æ
        {makePatternKey(R"(\HO)"), 0x0150}, // Ő
        {makePatternKey(R"(\HU)"), 0x0170}, // Ű
        {makePatternKey(R"(\Ho)"), 0x0151}, // ő
        {makePatternKey(R"(\Hu)"), 0x0171}, // ű
        {makePatternKey(R"(\OE)"), 0x0152}, // Œ

        {makePatternKey(R"(\^A)"), 0x00C2}, // Â
        {makePatternKey(R"(\^E)"), 0x00CA}, // Ê
        {makePatternKey(R"(\^I)"), 0x00CE}, // Î
        {makePatternKey(R"(\^O)"), 0x00D4}, // Ô
        {makePatternKey(R"(\^U)"), 0x00DB}, // Û
        {makePatternKey(R"(\^a)"), 0x00E2}, // â
        {makePatternKey(R"(\^e)"), 0x00EA}, // ê
        {makePatternKey(R"(\^i)"), 0x00EE}, // î
        {makePatternKey(R"(\^o)"), 0x00F4}, // ô
        {makePatternKey(R"(\^u)"), 0x00FB}, // û

        {makePatternKey(R"(\`?)"), 0x00BF}, // ¿
        {makePatternKey(R"(\`A)"), 0x00C0}, // À
        {makePatternKey(R"(\`E)"), 0x00C8}, // È
        {makePatternKey(R"(\`I)"), 0x00CC}, // Ì
        {makePatternKey(R"(\`O)"), 0x00D2}, // Ò
        {makePatternKey(R"(\`U)"), 0x00D9}, // Ù
        {makePatternKey(R"(\`a)"), 0x00E0}, // à
        {makePatternKey(R"(\`e)"), 0x00E8}, // è
        {makePatternKey(R"(\`i)"), 0x00EC}, // ì
        {makePatternKey(R"(\`o)"), 0x00F2}, // ò
        {makePatternKey(R"(\`u)"), 0x00F9}, // ù

        {makePatternKey(R"(\ae)"), 0x00E6}, // æ
        {makePatternKey(R"(\cC)"), 0x00C7}, // Ç
        {makePatternKey(R"(\cc)"), 0x00E7}, // ç
        {makePatternKey(R"(\cr)"), 0x000D}, // carriage return
        {makePatternKey(R"(\eu)"), 0x0080}, // €
        {makePatternKey(R"(\gl)"), 0x00AB}, // «
        {makePatternKey(R"(\gr)"), 0x00BB}, // »
        {makePatternKey(R"(\ht)"), 0x0009}, // tabulation
        {makePatternKey(R"(\lf)"), 0x000A}, // newline
        {makePatternKey(R"(\oO)"), 0x00D8}, // Ø
        {makePatternKey(R"(\oe)"), 0x0153}, // œ
        {makePatternKey(R"(\oo)"), 0x00F8}, // ø
        {makePatternKey(R"(\pn)"), 0x00A3}, // £
        {makePatternKey(R"(\rA)"), 0x00C5}, // Å
        {makePatternKey(R"(\ra)"), 0x00E5}, // å
        {makePatternKey(R"(\rq)"), 0x2019}, // ’
        {makePatternKey(R"(\ss)"), 0x00DF}, // ß           
        {makePatternKey(R"(\tm)"), 0x0099}, // ™

        {makePatternKey(R"(\~A)"), 0x00C3}, // Ã
        {makePatternKey(R"(\~N)"), 0x00D1}, // Ñ
        {makePatternKey(R"(\~O)"), 0x00D5}, // Õ
        {makePatternKey(R"(\~a)"), 0x00E3}, // ã
        {makePatternKey(R"(\~n)"), 0x00F1}, // ñ
        {makePatternKey(R"(\~o)"), 0x00F5}, // õ
    } };

    auto findReplacementForSpecialCharacter(utf8::utfchar32_t codepoint) { 
        return std::lower_bound(utf8ToPrintableCharacters.begin(), utf8ToPrintableCharacters.end(), codepoint, [](const auto& pair, utf8::utfchar32_t c) { 
            return pair.first < c; 
        }); 
    }

    auto findReplacementForPrintableCharacter(uint32_t patternCode) {
        return std::lower_bound(printableCharactersToUTF8.begin(), printableCharactersToUTF8.end(), patternCode, [](const auto& pair, uint32_t p) {
            return pair.first < p;
        });
    }
}

std::vector<unsigned char> MessageTransformation::getEncryptedMessage(std::string_view messageToEncrypt, std::span<const unsigned char> streamKey) {
	std::vector<unsigned char> encryptedMessage;
	encryptedMessage.reserve(messageToEncrypt.size());

    for (size_t i = 0; i < messageToEncrypt.size(); ++i) {
        const unsigned characterValue = static_cast<unsigned>(messageToEncrypt[i] + streamKey[i]);
        const unsigned char result = (characterValue > alphabetLength) ? characterValue - alphabetLength : characterValue;
        encryptedMessage.push_back(result);
    }
    return encryptedMessage;
}

std::string MessageTransformation::getDecryptionMessage(std::span<const unsigned char> messageToDecode, std::span<const unsigned char> streamKey) {
    std::string decodedMessage;
    decodedMessage.reserve(messageToDecode.size());

    for (size_t i = 0; i < messageToDecode.size(); ++i) {
        const int characterValue = static_cast<int>(messageToDecode[i] - streamKey[i]);
        const unsigned char result = (characterValue < 0) ? static_cast<unsigned char>(characterValue + alphabetLength) : static_cast<unsigned char>(characterValue);
		if (result >= alphabetLength) {
            return {};
		}
        decodedMessage += result;
    }
    return decodedMessage;
}

std::optional<std::vector<unsigned char>> MessageTransformation::createImage(std::string_view message, unsigned seed, const Image& image) {

    std::string messageToEncrypt = MessageTransformation::convertUTF8ToPrintableCharacters(message);
    const size_t imageSize = static_cast<size_t>(image.width) * image.height * image.channels;

    if ((messageToEncrypt.size() + HeaderData::HEADER_SIZE) > imageSize)
        return std::nullopt;

    SolitaireAlgorithm algorithm;
    algorithm.initializeDeck(seed);

    std::vector<unsigned char> streamKey = algorithm.getStreamKey(messageToEncrypt.size(), alphabetLength);
    std::vector<unsigned char> encryptedMessage = MessageTransformation::getEncryptedMessage(messageToEncrypt, streamKey);

    std::vector<unsigned char> imageData{};

    // If the user didn't provide any image to hide the encrypted message inside, the program will create a RGBA image
    if (!image.data.has_value()) {
        unsigned char type = 127; // Type of the image : 127 -> message hidden in a fully created image; 128 -> message hidden in an existing image
        unsigned stride = MessageTransformation::getRandomStride(HeaderData::HEADER_SIZE + sizeof(type), (image.width * image.height * image.channels) - static_cast<unsigned>(encryptedMessage.size()));

        const std::vector<unsigned char> header = ImageHeader::createHeader({ messageToEncrypt.size(), seed, stride });

        const size_t remainingCharactersLength = imageSize - (encryptedMessage.size() + HeaderData::HEADER_SIZE + sizeof(type));

        std::vector<unsigned char> remainingCharacters = MessageTransformation::generateRandomCharacters(remainingCharactersLength);
        unsigned char* remainingCharactersPtr = remainingCharacters.data();
        imageData.reserve(imageSize);

        imageData.push_back(type);
        imageData.insert(imageData.end(), header.data(), header.data() + header.size());
        imageData.insert(imageData.end(), remainingCharactersPtr, remainingCharactersPtr + stride);
        remainingCharactersPtr += stride;
        imageData.insert(imageData.end(), encryptedMessage.data(), encryptedMessage.data() + encryptedMessage.size());
        imageData.insert(imageData.end(), remainingCharactersPtr, remainingCharactersPtr + (remainingCharacters.size() - stride));
    }
    // otherwise, the user has provided an image. In this case, two scenarios : the image has 3 channels (RGB) or 4 (RGBA)
    else {
        unsigned char type = 128; // Type of the image : 127 -> message hidden in a fully created image; 128 -> message hidden in an existing image
        // If the image has 3 channels, the message will be stored contiguously in the image
        if (image.channels == 3) {
            unsigned stride = imageSize - encryptedMessage.size() - HeaderData::HEADER_SIZE - 1 - sizeof(type);
            std::vector<unsigned char> header = ImageHeader::createHeader({ messageToEncrypt.size(), seed, stride });

            imageData = *image.data;
            imageData[0] = type;
            std::copy(header.begin(), header.end(), imageData.begin() + sizeof(type));
            std::copy(encryptedMessage.begin(), encryptedMessage.end(), imageData.begin() + (header.size() + static_cast<size_t>(stride) + sizeof(type)));
        }
        // Else if the image has 4 channels, the message will be stored in the RGB channels, avoiding the alpha one
        else if (image.channels == 4) {
            unsigned stride = imageSize - encryptedMessage.size() - encryptedMessage.size() / 3 - HeaderData::HEADER_SIZE - 1 - sizeof(type);
            std::vector<unsigned char> header = ImageHeader::createHeader({ messageToEncrypt.size(), seed, stride });

            imageData = *image.data;
            imageData[0] = type;
            size_t headerPos = 0;
            // As we store the image type in the first channel of the image, then we start at the position 2 in the image and at the green channel of the first pixel in the image
            size_t imagePos = 1;
            size_t pixelPos = 2;
            for (; headerPos < HeaderData::HEADER_SIZE; ++imagePos) {
                if (pixelPos % 4 == 0) {
                    pixelPos = 1;
                    continue;
                }
                imageData[imagePos] = header[headerPos];
                headerPos++;
                pixelPos++;
            }

            imagePos = HeaderData::HEADER_SIZE + stride + sizeof(type);
            pixelPos = (HeaderData::HEADER_SIZE + stride + sizeof(type)) % 4 + 1;
            size_t encryptedMessagePos = 0;
            for (; encryptedMessagePos < encryptedMessage.size(); ++imagePos) {
                if (pixelPos % 4 == 0) {
                    pixelPos = 1;
                    continue;
                }
                imageData[imagePos] = encryptedMessage[encryptedMessagePos];
                encryptedMessagePos++;
                pixelPos++;
            }
        }
        else
            return std::nullopt;
    }

    return imageData;
}

std::optional<std::string> MessageTransformation::getMessageFromImage(std::span<const unsigned char> image, int numberOfChannels, std::string& errorMessage) {
    if (numberOfChannels < 3 || numberOfChannels > 4) {
        errorMessage = "The number of channels of the image must be 3 or 4.";
        return std::nullopt;
    }
    // Retrieve the first channel of the first pixel of the image (giving the type of image whom we will extract the encrypted message)
    unsigned char type = image[0];
    std::optional<std::string> decodedMessage;
    // If the message has been stored in a generated image
    if (type == 127)
        decodedMessage = MessageTransformation::getMessageFromGeneratedImage(image, errorMessage);
    // else if the message has been stored in an existing message
    else if (type == 128)
        decodedMessage = MessageTransformation::getMessageFromExistingImage(image, numberOfChannels, errorMessage);
    else {
        errorMessage = "The image does not contain any message.";
        return std::nullopt;
    }

    if (!decodedMessage.has_value())
        return std::nullopt;
    else
        return MessageTransformation::convertPrintableCharactersToUTF8(*decodedMessage);
}

std::vector<unsigned char> MessageTransformation::generateRandomCharacters(size_t charactersToGenerate)
{
    std::mt19937 gen{ std::random_device{}() };
    std::uniform_int_distribution<unsigned> dist(0, 255);

    std::vector<unsigned char> randomCharacters(charactersToGenerate);

    for (size_t i = 0; i < charactersToGenerate; ++i)
        randomCharacters[i] = dist(gen);

    return randomCharacters;
}

std::string MessageTransformation::convertUTF8ToPrintableCharacters(std::string_view message) {
    std::string outputMessage;
    outputMessage.reserve(message.size());

    // Convert each special character to a representation containing only printable characters
    for (auto it = message.begin(); it != message.end();) {
        auto start = it;

        utf8::utfchar32_t codepoint = utf8::next(it, message.end());
        // If the character is already a printable one
        if (codepoint > 0x001F && codepoint < 0x0080) {
            outputMessage.append(start, it);
        }
        // Else find it in the special characters table
        else {
            auto charSearch = findReplacementForSpecialCharacter(codepoint);

            if (charSearch != utf8ToPrintableCharacters.end() && charSearch->first == codepoint) {
                outputMessage += charSearch->second;
            }
            // Else the character is neither a printable character nor a referenced special character, so we replace the invalid one by a blank space
            else {
                outputMessage += ' ';
            }
        }
    }

    return outputMessage;
}

std::string MessageTransformation::convertPrintableCharactersToUTF8(std::string_view message) {
    // number of characters in special characters pattern
    static constexpr int patternLength = 3;
    std::string outputMessage;
    outputMessage.reserve(message.size());

    // Convert each special character to a representation containing only printable characters
    for (auto it = message.begin(); it != message.end();) {

        if (*it == '\\' && std::next(it, patternLength) <= message.end())
        {
            uint32_t patternCode =
                (static_cast<uint32_t>(static_cast<unsigned char>(it[0])) << 16) |
                (static_cast<uint32_t>(static_cast<unsigned char>(it[1])) << 8) |
                static_cast<uint32_t>(static_cast<unsigned char>(it[2]));

            auto patternSearch = findReplacementForPrintableCharacter(patternCode);

            if (patternSearch != printableCharactersToUTF8.end() &&
                patternSearch->first == patternCode)
            {
                utf8::append(patternSearch->second, outputMessage);
                it += patternLength;
                continue;
            }
        }

        outputMessage += *it;
        ++it;
    }

    return outputMessage;
}

unsigned MessageTransformation::getRandomStride(unsigned minStride, unsigned maxStride) {
	std::mt19937 gen(std::random_device{}());
	std::uniform_int_distribution<unsigned> dist(minStride, maxStride);
	return dist(gen);
}

std::optional<std::string> MessageTransformation::getMessageFromGeneratedImage(std::span<const unsigned char> image, std::string& errorMessage) {
    auto imagePtr = image.data() + sizeof(unsigned char);

    std::vector<unsigned char> rawHeader(imagePtr, imagePtr + HeaderData::HEADER_SIZE);
 
    std::optional<HeaderData> headerData = ImageHeader::getHeaderData(rawHeader, image.size());
    if (!headerData.has_value()) {
        errorMessage = "Unable to extract the header from the image.";
        return std::nullopt;
    }
    else {
        if ((*headerData).messageLength > static_cast<size_t>(image.size() - HeaderData::HEADER_SIZE)) {
            errorMessage = "Unable to extract a message from the image.\nThe message length is invalid or exceeds the image size.";
            return std::nullopt;
        }
        else if ((*headerData).messageLength == 0) {
            errorMessage = "The current image does not contain any message.";
            return std::nullopt;
        }
    }
    const size_t messageLength = (*headerData).messageLength;
    
    imagePtr += (HeaderData::HEADER_SIZE + (*headerData).stride);
    std::vector<unsigned char> rawMessage(imagePtr, imagePtr + messageLength);
    
    SolitaireAlgorithm algorithm;
    algorithm.initializeDeck((*headerData).seed);

    std::vector<unsigned char> streamKey = algorithm.getStreamKey(messageLength, alphabetLength);
    std::string decodedMessage = MessageTransformation::getDecryptionMessage(rawMessage, streamKey);
    if (decodedMessage.empty()) {
        errorMessage = "Unable to extract a message from the image.\nThe message is composed of invalid characters or\ncharacters not taken in charge by the decoding\nalgorithm.";
        return std::nullopt;
    }
    else
        return decodedMessage;
}

std::optional<std::string> MessageTransformation::getMessageFromExistingImage(std::span<const unsigned char> image, int numberOfChannels, std::string& errorMessage) {
    auto imagePtr = image.data() + sizeof(unsigned char);

    std::optional<std::vector<unsigned char>> rawHeader;
    if (numberOfChannels == 3)
        rawHeader.emplace(imagePtr, imagePtr + HeaderData::HEADER_SIZE);
    else {
        rawHeader.emplace(HeaderData::HEADER_SIZE);
        size_t headerPos = 0;
        size_t imagePos = 1;
        size_t pixelPos = 2;
        for (; headerPos < HeaderData::HEADER_SIZE; ++imagePos) {
            if (pixelPos % 4 == 0) {
                pixelPos = 1;
                continue;
            }
            (*rawHeader)[headerPos] = image[imagePos];
            headerPos++;
            pixelPos++;
        }
    }
    std::optional<HeaderData> headerData = ImageHeader::getHeaderData(*rawHeader, image.size());
    if (!headerData.has_value()) {
        errorMessage = "Unable to extract the header from the image.";
        return std::nullopt;
    }
    else {
        if ((*headerData).messageLength > static_cast<size_t>(image.size() - HeaderData::HEADER_SIZE)) {
            errorMessage = "Unable to extract a message from the image.\nThe message length is invalid or exceeds the image size.";
            return std::nullopt;
        }
        else if ((*headerData).messageLength == 0) {
            errorMessage = "The current image does not contain any message.";
            return std::nullopt;
        }
    }
    const size_t messageLength = (*headerData).messageLength;

    std::optional<std::vector<unsigned char>> rawMessage;
    if (numberOfChannels == 3) {
        imagePtr += (HeaderData::HEADER_SIZE + (*headerData).stride);
        rawMessage.emplace(imagePtr, imagePtr + messageLength);
    }
    else {
        rawMessage.emplace((*headerData).messageLength);
        size_t imagePos = HeaderData::HEADER_SIZE + sizeof(unsigned char) + (*headerData).stride;
        size_t pixelPos = (HeaderData::HEADER_SIZE + sizeof(unsigned char) + (*headerData).stride) % 4 + 1;
        size_t messagePos = 0;

        for (; messagePos < messageLength; ++imagePos) {
            if (pixelPos % 4 == 0) {
                pixelPos = 1;
                continue;
            }
            (*rawMessage)[messagePos] = image[imagePos];
            messagePos++;
            pixelPos++;
        }
    }
    SolitaireAlgorithm algorithm;
    algorithm.initializeDeck((*headerData).seed);

    std::vector<unsigned char> streamKey = algorithm.getStreamKey(messageLength, alphabetLength);
    std::string decodedMessage = MessageTransformation::getDecryptionMessage(*rawMessage, streamKey);
    if (decodedMessage.empty()) {
        errorMessage = "Unable to extract a message from the image.\nThe message is composed of invalid characters or\ncharacters not taken in charge by the decoding\nalgorithm.";
        return std::nullopt;
    }
    else
        return decodedMessage;
}
