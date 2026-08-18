#include "SolitaireUI.hpp"
#include "portable-file-dialogs.h"
#include "stb_image.h"

using namespace solitaire;

namespace {
    void generateImageTexture(GLuint& texture, const Image& image) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
		if (image.channels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, (*image.data).data());

        else if(image.channels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (*image.data).data());

        glTexParameteri(GL_TEXTURE_2D,
            GL_TEXTURE_MIN_FILTER,
            GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D,
            GL_TEXTURE_MAG_FILTER,
            GL_LINEAR);
    }

	void deleteImageTexture(GLuint& texture) {
		if (texture != 0) {
			glDeleteTextures(1, &texture);
			texture = 0;
		}
	}
}

SolitaireUI::SolitaireUI() : uiStateVariables{}, generatedImageData{}, userInputImageData{}, imageTexture(0), inputTextBuffer(65536, '\0'), inputFilepathBuffer{}, userInputImageBuffer{}, infosBuffer{}, seed{} {}

SolitaireUI::~SolitaireUI() {
	deleteImageTexture(this->imageTexture);
}

void SolitaireUI::resetUIParameters() {
	this->inputTextBuffer[0] = '\0';
	this->inputFilepathBuffer[0] = '\0';
    this->userInputImageBuffer[0] = '\0';
	this->infosBuffer[0] = '\0';
	this->seed = 0;
	deleteImageTexture(this->imageTexture);
    this->resetImageParameters(userInputImageData);
}

void SolitaireUI::resetImageParameters(Image& params) {
    params.data = std::nullopt;
    params.width = 400;
    params.height = 400;
    params.channels = 4;
}

void SolitaireUI::resetUIStateVariables() {
    this->uiStateVariables.encryptedMessageType = 0;
    this->uiStateVariables.inputTextFlags = ImGuiInputTextFlags_None;
    this->uiStateVariables.useExistingImage = false;
}

int SolitaireUI::init() {

    // GLFW initialisation
    if (!glfwInit())
        return -1;

    // OpenGL version 3.3 used here for the backend
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(825, 850, "Solitaire's Message Encryption & Decryption", nullptr, nullptr);
    if (window == nullptr)
        return 1;

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);


	// Imgui context creation
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Visual theme
    ImGui::StyleColorsDark();

    // Backend initialisation (platform + rendering)
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);

        ImGui::Begin("Encryption and decryption app", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        ImGui::SeparatorText("Mode");
		if (ImGui::RadioButton("Encryption", this->uiStateVariables.mode == 0)) {
			this->uiStateVariables.mode = 0;
            this->resetUIParameters();
            this->resetImageParameters(this->generatedImageData);
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Decryption", this->uiStateVariables.mode == 1)) {
			this->uiStateVariables.mode = 1;
            this->resetUIParameters();
            this->resetImageParameters(this->generatedImageData);
		}

		if (this->uiStateVariables.mode == 0)
			this->createEncryptionUI();
		else
			this->createDecryptionUI();
        ImGui::SeparatorText("Informations");
        ImGui::BeginChild("InformationBlock", ImVec2(810, 100), ImGuiChildFlags_Borders);

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.5f, 0.0f, 1.0f));
        ImGui::InputTextMultiline("##InformationText", this->infosBuffer, IM_ARRAYSIZE(this->infosBuffer), ImVec2(795, 80), ImGuiInputTextFlags_ReadOnly);
        ImGui::PopStyleColor();

        ImGui::EndChild();

        ImGui::Text("Average FPS : %.1f", io.Framerate);

        ImGui::End();

        // End of ImGui frame
        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.25f, 0.41f, 0.88f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Geometry RealTime rendering
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Application display
        glfwSwapBuffers(window);
    }

    // Clean texture(s), shutdown GLFW + openGL and destroy Imgui context + GLFW window
    deleteImageTexture(this->imageTexture);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void SolitaireUI::createEncryptionUI() {
    ImGui::SeparatorText("Message Source :");

    // ** Radio button for user input **
    if (ImGui::RadioButton("User input", this->uiStateVariables.encryptedMessageType == 0)) {
        this->uiStateVariables.encryptedMessageType = 0;

        this->uiStateVariables.inputTextFlags = ImGuiInputTextFlags_None;
        this->resetUIParameters();
    }
    ImGui::SameLine();

    // ** Radio button for text file input **
    if (ImGui::RadioButton("Text file", this->uiStateVariables.encryptedMessageType == 1)) {
        this->uiStateVariables.encryptedMessageType = 1;

        this->uiStateVariables.inputTextFlags = ImGuiInputTextFlags_ReadOnly;
        this->resetUIParameters();
    }

    // ** Case when the 'text file' radio button is selected **
    ImGui::BeginDisabled(this->uiStateVariables.encryptedMessageType != 1);
    ImGui::InputText("##Filepath", this->inputFilepathBuffer, IM_ARRAYSIZE(this->inputFilepathBuffer), this->uiStateVariables.inputTextFlags);
    ImGui::SameLine();
    if (ImGui::Button("Load text file")) {
		this->inputFilepathBuffer[0] = '\0';
        this->infosBuffer[0] = '\0';
        auto selection = pfd::open_file("Select a text file", ".", { "Text files (*.txt)", "*.txt" }).result();
        if (!selection.empty())
            snprintf(this->inputFilepathBuffer, sizeof(this->inputFilepathBuffer), "%s", selection[0].c_str());
        if (strlen(this->inputFilepathBuffer) > 0) {
            const std::optional<std::string> fileContent = FileManager::loadFile(this->inputFilepathBuffer);
            if (!fileContent.has_value())
                snprintf(this->infosBuffer, sizeof(this->infosBuffer), "%s\n\nFile '%s' failed to load.", ErrorHandler::LOADING_FILE_ERROR, selection[0].c_str());
            else
                snprintf(this->inputTextBuffer.data(), this->inputTextBuffer.size(), "%s", (*fileContent).c_str());
        }
    }
    ImGui::EndDisabled();

    // ** Seed range selecton **
    ImGui::InputInt("Seed to generate the stream key", &this->seed);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("The stream key is used to encrypt the message.\nIf the value is set to 0, the seed will be randomly chosen.");
    if (this->seed < 0) this->seed = 0;

    // ** Generated image width and height personalization
    ImGui::BeginDisabled(this->uiStateVariables.useExistingImage);
    ImGui::InputInt("Image width", &this->generatedImageData.width);
    if (this->generatedImageData.width < 0) this->generatedImageData.width = 0;
    else if (this->generatedImageData.width > 4096) this->generatedImageData.width = 4096;
    ImGui::InputInt("Image height", &this->generatedImageData.height);
    if (this->generatedImageData.height < 0) this->generatedImageData.height = 0;
    else if (this->generatedImageData.height > 4096) this->generatedImageData.height = 4096;
    ImGui::EndDisabled();

    // ** CheckBox for loading existing image **
    if (ImGui::Checkbox("Use existing image ?", &this->uiStateVariables.useExistingImage)) {
        this->resetImageParameters(this->generatedImageData);
    }

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Turn on this option if you want to hide\nyour message inside an existing image.");

    // ** Case when the loading existing image checkbox is checked **
    ImGui::BeginDisabled(!this->uiStateVariables.useExistingImage);
    ImGui::SameLine();
    ImGui::InputText("##ImageInput", this->userInputImageBuffer, IM_ARRAYSIZE(this->userInputImageBuffer), ImGuiInputTextFlags_ReadOnly);
    ImGui::SameLine();
    if (ImGui::Button("Load Image##UserInput")) {
        this->infosBuffer[0] = '\0';
        this->userInputImageBuffer[0] = '\0';
        auto selection = pfd::open_file("Select an image file", ".", { "Image files (*.png, *.jpg, *.jpeg)", "*.png *.jpg *.jpeg" }).result();
        if (!selection.empty()) {
            if (stbi_is_16_bit(selection[0].c_str()))
                snprintf(this->infosBuffer, sizeof(this->infosBuffer), "%s", ErrorHandler::LOADING_16_BITS_IMAGE_ERROR);
            else
                snprintf(this->userInputImageBuffer, sizeof(this->userInputImageBuffer), "%s", selection[0].c_str());
        }
    }
    ImGui::EndDisabled();

    // ** Block for the message to encrypt **
    ImGui::BeginChild("EncryptMessage", ImVec2(400, 440), ImGuiChildFlags_Borders);
    ImGui::SeparatorText("Message to encrypt");

    ImGui::InputTextMultiline("##MessageToEncrypt", this->inputTextBuffer.data(), this->inputTextBuffer.size(), ImVec2(400, 400), this->uiStateVariables.inputTextFlags);
    ImGui::EndChild();

    ImGui::SameLine();

    // ** Block for the resulting image after the encryption **
    ImGui::BeginChild("ImageRender", ImVec2(400, 440), ImGuiChildFlags_Borders);
    ImGui::SeparatorText("Image result");
    if (this->imageTexture != 0)
        ImGui::Image((ImTextureID)(intptr_t)this->imageTexture, ImVec2(400, 400));

    ImGui::EndChild();

    // ** Length of message to encrypt **
	ImGui::Text("Message length : %llu characters", strlen(this->inputTextBuffer.data()));

    // ** Encryption button **
    ImGui::BeginDisabled(this->inputTextBuffer[0] == '\0');
    if (ImGui::Button("Encrypt message")) {
        this->generatedImageData.data = std::nullopt;
        this->userInputImageData.data = std::nullopt;
        this->infosBuffer[0] = '\0';
        std::string_view messageToEncrypt(this->inputTextBuffer.data());
        deleteImageTexture(this->imageTexture);
        if(this->uiStateVariables.useExistingImage){
            // Try to load user input image whom purpose is to hide the encrypted message inside
            this->userInputImageData = FileManager::loadImage(this->userInputImageBuffer);
            if (!this->userInputImageData.data.has_value())
                snprintf(this->infosBuffer, sizeof(this->infosBuffer), "%s\n\nImage '%s' failed to load.\nMake sure that the filepath is correct and the image has 3 or 4 channels.", ErrorHandler::LOADING_FILE_ERROR, this->inputFilepathBuffer);
            else {
                this->generatedImageData.width = this->userInputImageData.width;
                this->generatedImageData.height = this->userInputImageData.height;
                this->generatedImageData.channels = this->userInputImageData.channels;
                this->generatedImageData.data = MessageTransformation::createImage(messageToEncrypt, static_cast<unsigned char>(this->seed), this->userInputImageData);
                if (!this->generatedImageData.data.has_value())
                    snprintf(this->infosBuffer, sizeof(this->infosBuffer), "%s\n\nThe message to encrypt is too large for the chosen image.", ErrorHandler::MESSAGE_LENGTH_ERROR);
                else
                    generateImageTexture(this->imageTexture, this->userInputImageData);
            }
        }
        else {
            this->generatedImageData.data = MessageTransformation::createImage(messageToEncrypt, static_cast<unsigned char>(this->seed), this->generatedImageData);
            if (!this->generatedImageData.data.has_value())
                snprintf(this->infosBuffer, sizeof(this->infosBuffer), "%s\n\nThe message to encrypt is too large for the chosen image.", ErrorHandler::MESSAGE_LENGTH_ERROR);
            else
                generateImageTexture(this->imageTexture, this->generatedImageData);
        }
    }
    ImGui::EndDisabled();

    ImGui::SameLine();
    // ** Reset button **
    if (ImGui::Button("Reset")) {
        this->resetUIParameters();
        this->resetUIStateVariables();
        this->resetImageParameters(generatedImageData);
    }
    ImGui::SameLine();

    // ** Save image button **
    ImGui::BeginDisabled(this->imageTexture == 0);
    if (ImGui::Button("Save image")) {
        auto selection = pfd::save_file("Save image as", "", { "Image files (*.png)", "*.png" }).result();
        if (!selection.empty()) {
            this->infosBuffer[0] = '\0';
            if (!selection.ends_with(".png"))
                snprintf(this->infosBuffer, sizeof(infosBuffer), "%s\n\nFile '%s' must have a '.png' extension.", ErrorHandler::FILE_FORMAT_ERROR, selection.c_str());
            else {
                int result = FileManager::saveMessageAsImage(selection.c_str(), this->generatedImageData);
                if (!result)
                    snprintf(this->infosBuffer, sizeof(this->infosBuffer), "%s\n\nFile '%s' saving failed.", ErrorHandler::SAVING_FILE_ERROR, selection.c_str());
                else
                    snprintf(this->infosBuffer, sizeof(this->infosBuffer), "Image '%s'\nhas been saved successfully.", selection.c_str());
                deleteImageTexture(this->imageTexture);
                this->generatedImageData.data = std::nullopt;
            }
        }
    }
    ImGui::EndDisabled();
}

void SolitaireUI::createDecryptionUI() {
    // ** Input text for the filepath of the image to decrypt **
    ImGui::InputText("##Filepath", this->inputFilepathBuffer, IM_ARRAYSIZE(this->inputFilepathBuffer), this->uiStateVariables.inputTextFlags);
    ImGui::SameLine();
    // ** Load image button **
    if (ImGui::Button("Load Image")) {
		this->resetUIParameters();
        auto selection = pfd::open_file("Select an image file", ".", { "Image files (*.png)", "*.png" }).result();
        if (!selection.empty()) {
			if (stbi_is_16_bit(selection[0].c_str()))
				snprintf(this->infosBuffer, sizeof(this->infosBuffer), "%s", ErrorHandler::LOADING_16_BITS_IMAGE_ERROR);
            else
                snprintf(this->inputFilepathBuffer, sizeof(this->inputFilepathBuffer), "%s", selection[0].c_str());
        }
        if (strlen(this->inputFilepathBuffer) > 0) {
            deleteImageTexture(this->imageTexture);
			this->resetImageParameters(generatedImageData);
            this->generatedImageData = FileManager::loadImage(this->inputFilepathBuffer);
            if (!this->generatedImageData.data.has_value())
                snprintf(this->infosBuffer, sizeof(this->infosBuffer), "%s\n\nImage '%s' failed to load.\nMake sure that the filepath is correct and the image has 3 or 4 channels.", ErrorHandler::LOADING_FILE_ERROR, this->inputFilepathBuffer);
            else
                generateImageTexture(this->imageTexture, this->generatedImageData);
        }
    }

    // ** Block to display image to decrypt **
    ImGui::BeginChild("ImageToDecrypt", ImVec2(400, 440), ImGuiChildFlags_Borders);
    ImGui::SeparatorText("Image to decrypt");

    if (this->imageTexture != 0)
        ImGui::Image((ImTextureID)(intptr_t)this->imageTexture, ImVec2(400, 400));

    ImGui::EndChild();

    ImGui::SameLine();

    // ** Block for the decrypted message **
    ImGui::BeginChild("DecryptedMessage", ImVec2(400, 440), ImGuiChildFlags_Borders);
    ImGui::SeparatorText("Decrypted Message");

    ImGui::InputTextMultiline("##DecryptedMessage", this->inputTextBuffer.data(), this->inputTextBuffer.size(), ImVec2(400, 400), ImGuiInputTextFlags_ReadOnly);

    ImGui::EndChild();

    // ** Length of the decrypted message **
    ImGui::Text("Decrypted message length : %llu characters", strlen(this->inputTextBuffer.data()));

    ImGui::BeginDisabled(this->imageTexture == 0);
    // ** Decrypt message button **
    if (ImGui::Button("Decrypt message")) {
        this->inputTextBuffer[0] = '\0';
        this->infosBuffer[0] = '\0';
        std::string errorMessage;
		const std::optional<std::string> decryptedMessage = MessageTransformation::getMessageFromImage(*this->generatedImageData.data, this->generatedImageData.channels, errorMessage);
        if (!decryptedMessage.has_value())
            snprintf(this->infosBuffer, sizeof(infosBuffer), "%s\n\n%s", ErrorHandler::DECRYPTED_MESSAGE_ERROR, errorMessage.c_str());
        else
            snprintf(this->inputTextBuffer.data(), this->inputTextBuffer.size(), "%s", (*decryptedMessage).c_str());
    }
    ImGui::EndDisabled();

    ImGui::SameLine();
    // ** Reset button **
    if (ImGui::Button("Reset")) {
        this->resetUIParameters();
        this->resetUIStateVariables();
    } 
    ImGui::SameLine();

    ImGui::BeginDisabled(this->inputTextBuffer[0] == '\0' || this->imageTexture == 0);
    // ** Export decrypted message button **
    if (ImGui::Button("Export Decrypted Message")) {
        auto selection = pfd::save_file("Save decrypted message as", "", { "Text files (*.txt)", "*.txt" }).result();
        if (!selection.empty()) {
            if (!selection.ends_with(".txt"))
                snprintf(this->infosBuffer, sizeof(this->infosBuffer), "%s\n\nFile '%s' must have a '.txt' extension.", ErrorHandler::FILE_FORMAT_ERROR, selection.c_str());
            else {
                int result = FileManager::saveMessageAsFile(selection.c_str(), this->inputTextBuffer.data());
                if (!result)
                    snprintf(this->infosBuffer, sizeof(this->infosBuffer), "%s\n\nFile '%s' saving failed.", ErrorHandler::SAVING_FILE_ERROR, selection.c_str());
                else {
                    snprintf(this->infosBuffer, sizeof(this->infosBuffer), "Decrypted message saved in file '%s'.", selection.c_str());
                    this->inputTextBuffer[0] = '\0';
                }
            }
        }
    }
    ImGui::EndDisabled();
}
