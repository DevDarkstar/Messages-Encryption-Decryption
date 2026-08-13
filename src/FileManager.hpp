#pragma once

#include <string>
#include <vector>
#include <optional>

namespace solitaire {

	// Struct to store the image data to display in the ImGui window (either the input image to decrypt or the image generated after encryption)
	struct Image {
		std::optional<std::vector<unsigned char>> data;
		int width = 400;
		int height = 400;
		int channels = 3;
	};

	class FileManager {
	public:
		static std::optional<std::string> loadFile(const char* filepath);
		static Image loadImage(const char* filepath);
		static int saveMessageAsFile(const char* filename, std::string_view message);
		static int saveMessageAsImage(const char* filename, const Image& image);
	};
}
