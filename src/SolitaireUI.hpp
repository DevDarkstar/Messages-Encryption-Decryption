#pragma once

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"		
#include "imgui_impl_opengl3.h"
#include "MessageTransformation.hpp"
#include "FileManager.hpp"

namespace solitaire {

	// Struct to store the state variables for the ImGui window
	struct UIStateVariables {
		//Mode
		// 0 -> encryption mode;
		// 1 -> decryption mode.
		int mode = 0;
		// Source of message to encrypt
		// 0 -> message from user input;
		// 1 -> message from .txt file.
		int encryptedMessageType = 0;
		// Flags for the ImGui input text field to enter the message to encrypt or the filepath of the input file (text in encryption mode or image in decryption mode).
		// ImGuiInputTextFlags_None -> message from user input (encryption mode);
		// ImGuiInputTextFlags_ReadOnly -> message from .txt file (encryption mode) or decrypted message from image (decryption mode).
		ImGuiInputTextFlags inputTextFlags = ImGuiInputTextFlags_None;
		bool useExistingImage = false;
	};

	// Struct to store message error the application can encounter
	struct ErrorHandler {
		inline static const char* const LOADING_16_BITS_IMAGE_ERROR = "Err01 : 16 bits depth color images are not supported by the application.";
		inline static const char* const LOADING_FILE_ERROR = "Err02 : file load error";
		inline static const char* const SAVING_FILE_ERROR = "Err03 : file save error";
		inline static const char* const FILE_FORMAT_ERROR = "Err04 : file format error";
		inline static const char* const DECRYPTED_MESSAGE_ERROR = "Err05 : decrypted message error";
		inline static const char* const MESSAGE_LENGTH_ERROR = "Err06 : message length error";
	};

	class SolitaireUI {
		public:
			SolitaireUI();
			~SolitaireUI();
			/**
			** @brief Initialize the ImGui window.
			*/
			int init();
		private:
			// State variables for the ImGui window
			UIStateVariables uiStateVariables;
			// Image to display in the ImGui window (either the input image to decrypt or the image generated after encryption)
			Image generatedImageData;
			// Image used to hide the encrypted message inside if the option is selected
			Image userInputImageData;
			// OpenGL texture ID for the image to display in the ImGui window (either the image to decrypt or the image generated after encryption)
			GLuint imageTexture;
			// Buffer to store the message to encrypt or the decrypted message
			std::vector<char> inputTextBuffer;
			// Buffer to store the filepath of the input file (text in encryption mode or image in decryption mode)
			char inputFilepathBuffer[128];
			// Buffer to store the filepath of the input  image in encrytion mode, to store an encrypted message inside
			char userInputImageBuffer[128];
			// Buffer to store error messages or information messages to display in the ImGui window
			char infosBuffer[256];
			// Seed used to generate the stream key for encryption
			int seed;
			/**
			** @brief Reset ImGui window parameters (images data, state variables and attributes)
			*/
			void resetUIParameters();
			/**
			** @brief Reset the image parameters to their default values.
			** @param params the image data parameters to reinitialize 
			*/
			void resetImageParameters(Image& params);
			/**
			** @brief Reset the state variables (from UIStateVariables struct) to their default values.
			*/
			void resetUIStateVariables();
			/**
			** @brief Create the UI for the encryption mode.
			*/
			void createEncryptionUI();
			/**
			** @brief Create the UI for the decryption mode.
			*/
			void createDecryptionUI();
	};
}
