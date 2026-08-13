#include "FileManager.hpp"
#include <fstream>
#include "stb_image.h"
#include "stb_image_write.h"

using namespace solitaire;

std::optional<std::string> FileManager::loadFile(const char* filepath) {

    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        file.close();
        return std::nullopt;
    }

    std::streamsize size = static_cast<std::streamsize>(file.tellg());
    file.seekg(0, std::ios::beg);

    std::string message(size, '\0');
    file.read(message.data(), size);
    if (!file) {
        file.close();
        return std::nullopt;
    }
    file.close();
    return message;
}

Image FileManager::loadImage(const char* filepath) {
    Image img;
	unsigned char* data = stbi_load(filepath, &img.width, &img.height, &img.channels, 0);

    if (!data) {
        img.data = std::nullopt;
        return img;
    }

    if (img.channels < 3 || img.channels > 4)
        img.data = std::nullopt;
    else {
        img.data.emplace(img.width * img.height * img.channels);
        memcpy((*img.data).data(), data, sizeof(unsigned char) * img.width * img.height * img.channels);
    }
	stbi_image_free(data);
    return img;
}


int FileManager::saveMessageAsFile(const char* filename, std::string_view message) {

    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return 0;
    }

    int result;
    file.write(message.data(), static_cast<std::streamsize>(message.size()));

    if (file)
        result = 1;
    else
        result = 0;
    file.close();
    return result;
}

int FileManager::saveMessageAsImage(const char* filename, const Image& image) {

	return stbi_write_png(filename, image.width, image.height, image.channels, (*image.data).data(), image.width * image.channels);
}
