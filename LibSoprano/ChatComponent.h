// Copyright James Puleo 2021
// Copyright LibSoprano 2021

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

#pragma once
#include "result.h"
#include "Color.h"
#include <string>
#include <optional>
#include <json.hpp>
#include <imgui/imgui.h>

namespace LibSoprano
{
    class ChatComponent
    {
    public:
        using Error = std::string;

        enum class Type
        {
            String,
            Translation,
            Keybind,
            Score,
            Selector
        };

        struct FontOptions
        {
            ImFont* regular = nullptr;
            ImFont* bold = nullptr;
            ImFont* italic = nullptr;
            ImFont* bold_italic = nullptr;
        };

        static Result<ChatComponent, Error> parse(std::string&);
        static Result<ChatComponent, Error> parse(nlohmann::json&);

        const std::optional<bool>& bold() const { return m_bold; }
        const std::optional<bool>& italic() const { return m_italic; }
        const std::optional<bool>& underlined() const { return m_underlined; }
        const std::optional<bool>& strikethrough() const { return m_strikethrough; }
        const std::optional<bool>& obfuscated() const { return m_obfuscated; }
        const std::optional<Color>& color() const { return m_color; }
        Type type() { return m_type; }
        const std::string& text() const { return m_text; }
        const std::vector<ChatComponent>& children() const { return m_children; }

        std::string to_ansi_string(bool escape = false, bool reset = true, ChatComponent* parent = nullptr);
        std::string to_html_string();
        void draw_imgui(FontOptions* = nullptr, bool same_line = false);
    private:
        // These are optional as they might not be present.
        // If they aren't present, they either inherit their
        // parent, or defaults to false.
        std::optional<bool> m_bold;
        std::optional<bool> m_italic;
        std::optional<bool> m_underlined;
        std::optional<bool> m_strikethrough;
        std::optional<bool> m_obfuscated;
        std::optional<Color> m_color;
        Type m_type;
        std::string m_text;

        std::vector<ChatComponent> m_children;
    };
}
