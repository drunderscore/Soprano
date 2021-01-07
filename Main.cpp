// Copyright James Puleo 2021
// Copyright Soprano 2021

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <LibSoprano/ChatComponent.h>
#include <fstream>
#include <sstream>
#include <imgui/imgui.h>
#include <GL/gl3w.h>
#include <imgui/backends/imgui_impl_sdl.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <SDL.h>
#include <cxxopts.hpp>

constexpr float FONT_SIZE = 15.0f;

static SDL_Window* s_window = nullptr;
static SDL_GLContext s_gl_context = nullptr;
static bool s_closing = false;
using ChatComponentResult = Result<LibSoprano::ChatComponent, LibSoprano::ChatComponent::Error>;
static std::shared_ptr<ChatComponentResult> s_active_component;
static bool s_window_open = true;
static char s_json_buffer[1024 * 16] = R"({
    "text": "Hello! I am ",
    "extra":
    [
        {
            "text": "Soprano.",
            "color": "gold"
        }
    ]
})";
static LibSoprano::ChatComponent::FontOptions s_font_options;

void parse_json()
{
    std::string json_str = s_json_buffer;
    s_active_component = std::make_shared<ChatComponentResult>(LibSoprano::ChatComponent::parse(json_str));
}

bool poll()
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        ImGui_ImplSDL2_ProcessEvent(&e);

        if (e.type == SDL_QUIT)
            s_closing = true;
        else if(e.type == SDL_KEYDOWN)
        {
            if(e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                s_closing = true;
        }
    }

    return s_closing;
}

void draw()
{
    ImGui::ShowDemoWindow();

    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 200), ImVec2(FLT_MAX, FLT_MAX));
    if(ImGui::Begin("Soprano", &s_window_open))
    {
        if(ImGui::InputTextMultiline("##Chat Component Text Input", s_json_buffer, sizeof(s_json_buffer),
                                     ImVec2(ImGui::GetContentRegionAvailWidth(),
                                            ImGui::GetContentRegionAvail().y / 1.5f), ImGuiInputTextFlags_AllowTabInput))
        {
            parse_json();
        }

        if(ImGui::BeginChild("Chat Component Text", ImVec2(0, 0), true))
        {
            if(s_active_component && s_active_component->isOk())
                s_active_component->storage().get<LibSoprano::ChatComponent>().draw_imgui(&s_font_options);
            else
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                ImGui::TextWrapped("Parse error: %s", s_active_component->storage().get<std::string>().c_str());
                ImGui::PopStyleColor();
            }
        }

        if (s_active_component && s_active_component->isOk() && ImGui::BeginPopupContextWindow())
        {
            if(ImGui::Selectable("Copy as ANSI"))
                ImGui::SetClipboardText(s_active_component->storage().get<LibSoprano::ChatComponent>().to_ansi_string(true).c_str());

            if(ImGui::Selectable("Copy as HTML"))
                ImGui::SetClipboardText(s_active_component->storage().get<LibSoprano::ChatComponent>().to_html_string().c_str());

            ImGui::EndPopup();
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

int main(int argc, char** argv)
{
    // TOOD: Allow this to be invoked commandline-wise for HTML and ANSI output.

    cxxopts::Options options(*argv, "Build and visualize Minecraft chat components");
    options.add_options()
            ("a,ansi",      "Output the chat component with ANSI sequences and exit")
            ("e,escansi",   "Output the chat component with ANSI escape sequences (C++) and exit")
            ("h,html",      "Output the chat component as HTML and exit")
            ("help",        "Shows help and exits")
            ("i,input",     "The JSON chat component", cxxopts::value<std::string>());

    options.positional_help("[chat component]").show_positional_help();

    options.parse_positional("input");
    try
    {
        auto res = options.parse(argc, argv);
        if(res.count("help"))
        {
            printf("%s\n", options.help().c_str());
            return 0;
        }

        if(res.count("input"))
            strcpy_s(s_json_buffer, res["input"].as<std::string>().c_str());

        if(strlen(s_json_buffer) > 0)
            parse_json();

        if((res.count("ansi") || res.count("escansi") || res.count("html")) && s_active_component->isErr())
        {
            fprintf(stderr, "%s\n", s_active_component->storage().get<std::string>().c_str());
            return 2;
        }

        if(res.count("ansi"))
        {
            printf("%s\n", s_active_component->storage().get<LibSoprano::ChatComponent>().to_ansi_string().c_str());
            return 0;
        }

        if(res.count("escansi"))
        {
            printf("%s\n", s_active_component->storage().get<LibSoprano::ChatComponent>().to_ansi_string(true).c_str());
            return 0;
        }

        if(res.count("html"))
        {
            printf("%s\n", s_active_component->storage().get<LibSoprano::ChatComponent>().to_html_string().c_str());
            return 0;
        }
    }
    catch(const cxxopts::OptionException& e)
    {
        fprintf(stderr, "%s\n", e.what());
        printf("%s\n", options.help().c_str());
        return 1;
    }

#if __APPLE__
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    s_window = SDL_CreateWindow("Soprano (hidden)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720,
                                SDL_WindowFlags::SDL_WINDOW_OPENGL |
                                SDL_WindowFlags::SDL_WINDOW_RESIZABLE |
                                SDL_WindowFlags::SDL_WINDOW_ALLOW_HIGHDPI |
                                SDL_WindowFlags::SDL_WINDOW_HIDDEN);

    s_gl_context = SDL_GL_CreateContext(s_window);
    SDL_GL_MakeCurrent(s_window, s_gl_context);
    SDL_GL_SetSwapInterval(0);

    if(gl3wInit() != 0)
    {
        fprintf(stderr, "Couldn't initialize GL3W\n");
        return 4;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGui::GetIO().ConfigViewportsNoAutoMerge = true;
    ImGui_ImplSDL2_InitForOpenGL(s_window, s_gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    auto io = &ImGui::GetIO();
    io->Fonts->AddFontDefault();
    s_font_options.regular = io->Fonts->AddFontFromFileTTF("C:/Windows/Fonts/consola.ttf", FONT_SIZE);
    s_font_options.bold = io->Fonts->AddFontFromFileTTF("C:/Windows/Fonts/consolab.ttf", FONT_SIZE);
    s_font_options.italic = io->Fonts->AddFontFromFileTTF("C:/Windows/Fonts/consolai.ttf", FONT_SIZE);
    s_font_options.bold_italic = io->Fonts->AddFontFromFileTTF("C:/Windows/Fonts/consolaz.ttf", FONT_SIZE);

    while(!s_closing)
    {
        if(poll())
            continue;

        if(!s_window_open)
        {
            s_closing = true;
            continue;
        }

        auto io = ImGui::GetIO();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(s_window);
        ImGui::NewFrame();

        draw();

        ImGui::Render();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();

        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(s_window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(s_gl_context);
    SDL_DestroyWindow(s_window);
    return 0;
}
