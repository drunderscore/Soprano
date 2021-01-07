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

#include "ChatComponent.h"
#include <fmt/format.h>
#include <sstream>
#include <imgui/imgui_internal.h>
#include <climits>

using namespace nlohmann;

namespace LibSoprano
{
    // https://stackoverflow.com/questions/105252/how-do-i-convert-between-big-endian-and-little-endian-values-in-c
    template <typename T>
    static T swap_endian(T u)
    {
        static_assert (CHAR_BIT == 8, "CHAR_BIT != 8");

        union
        {
            T u;
            unsigned char u8[sizeof(T)];
        } source, dest;

        source.u = u;

        for (size_t k = 0; k < sizeof(T); k++)
            dest.u8[k] = source.u8[sizeof(T) - k - 1];

        return dest.u;
    }

    Result<ChatComponent, ChatComponent::Error> ChatComponent::parse(std::string& raw_json)
    {
        json json;
        try
        {
            json = json::parse(raw_json);
            if(!json.is_object())
                return Err(std::string("Component must be an object"));
        }
        catch(json::exception& e)
        {
            return Err(std::string(e.what()));
        }

        return parse(json);
    }

    // TODO: Some annoying code duplication... could template some of this
    // (or macros if we were so evil)
    Result<ChatComponent, ChatComponent::Error> ChatComponent::parse(json& json)
    {
        ChatComponent comp;

        if(json.contains("text"))
        {
            auto val = json["text"];
            if(val.is_string())
            {
                comp.m_type = Type::String;
                comp.m_text = val.get<std::string>();
            }
        }
        else
        {
            return Err(std::string("Imcomplete or unsupported component"));
        }

        if(json.contains("bold"))
        {
            auto val = json["bold"];
            if(!val.is_boolean())
                return Err(std::string("Property \"bold\" must be a boolean"));
            comp.m_bold = val.get<bool>();
        }

        if(json.contains("italic"))
        {
            auto val = json["italic"];
            if(!val.is_boolean())
                return Err(std::string("Property \"italic\" must be a boolean"));
            comp.m_italic = val.get<bool>();
        }

        if(json.contains("underlined"))
        {
            auto val = json["underlined"];
            if(!val.is_boolean())
                return Err(std::string("Property \"underlined\" must be a boolean"));
            comp.m_underlined = val.get<bool>();
        }

        if(json.contains("strikethrough"))
        {
            auto val = json["strikethrough"];
            if(!val.is_boolean())
                return Err(std::string("Property \"strikethrough\" must be a boolean"));
            comp.m_strikethrough = val.get<bool>();
        }

        if(json.contains("obfuscated"))
        {
            auto val = json["obfuscated"];
            if(!val.is_boolean())
                return Err(std::string("Property \"obfuscated\" must be a boolean"));
            comp.m_obfuscated = val.get<bool>();
        }

        if(json.contains("color"))
        {
            auto val = json["color"];
            if(val.is_string())
            {
                auto col_str = val.get<std::string>();
                if(col_str.rfind("#") == 0)
                {
                    unsigned long color;
                    try
                    {
                        // FIXME: This won't throw if it encounters a non-base-16 character
                        // after the first character, and will simply return the value up
                        // until encountering it. That's not right! (I'm not even sure the
                        // use case for that...)
                        color = std::stoul(col_str.substr(1), nullptr, 16);
                    }
                    catch(std::logic_error&)
                    {
                        return Err(fmt::format("Invalid \"color\" hexadecimal property ({})", col_str));
                    }

                    comp.m_color = Color(color);
                }
                else
                {
                    auto color = Color::from_name(col_str.c_str());
                    if(!color)
                        return Err(fmt::format("Invalid \"color\" name property ({})", col_str));
                    comp.m_color = *color;
                }
            }
        }

        if(json.contains("extra"))
        {
            auto val = json["extra"];
            if(!val.is_array())
                return Err(std::string("Property \"extra\" must be an array"));

            for(auto comp_json : val)
            {
                auto child_comp = ChatComponent::parse(comp_json);
                if(child_comp.isErr())
                    return child_comp;

                comp.m_children.push_back(child_comp.unwrap());
            }
        }

        return Ok(comp);
    }

    std::string ChatComponent::to_ansi_string(bool escape, bool reset, ChatComponent* parent)
    {
        auto use_color = m_color && m_color->ansi_color();
        std::stringstream buffer;

        auto insert_escape = [escape, &buffer]()
        {
            if(escape)
                buffer << Color::m_ansi_escape_escaped;
            else
                buffer << Color::m_ansi_escape;
        };

        if(use_color)
        {
            insert_escape();
            buffer << m_color->ansi_color();
        }

        buffer << m_text;

        for(auto comp : m_children)
        {
            buffer << comp.to_ansi_string(escape, false, this);
            if(use_color && parent && parent->color() && parent->color()->ansi_color())
            {
                insert_escape();
                buffer << parent->color()->ansi_color();
            }
        }

        if(reset)
        {
            insert_escape();
            buffer << Color::m_ansi_reset;
        }

        return buffer.str();
    }

    std::string ChatComponent::to_html_string()
    {
        std::stringstream html_buffer;
        std::stringstream style_buffer;

        html_buffer << "<span style=\"";

        if(m_color)
            style_buffer << fmt::format("color: #{:x};", m_color->foreground());

        if(m_bold)
        {
            style_buffer << "font-weight: ";
            if(*m_bold)
                style_buffer << "bold;";
            else
                style_buffer << "normal;";
        }

        if(m_italic)
        {
            style_buffer << "font-style: ";
            if(*m_italic)
                 style_buffer << "italic;";
            else
                style_buffer << "normal;";
        }

        if(m_underlined)
        {
            style_buffer << "text-decoration-line: ";
            if(*m_underlined)
                 style_buffer << "underline;";
            else
                style_buffer << "inherit;";
        }

        html_buffer << style_buffer.str() << "\">" << m_text;

        for(auto comp : m_children)
            html_buffer << comp.to_html_string();

        html_buffer << "</span>";

        return html_buffer.str();
    }

    void ChatComponent::draw_imgui(ChatComponent::FontOptions* font_opts, bool same_line)
    {
        if(m_color)
            ImGui::PushStyleColor(ImGuiCol_Text, (swap_endian(m_color->foreground()) >> 8) | 0xFF000000);

        // TODO: make this code disappear. it's hideous.
        if(bold() && !italic() && *bold())
            ImGui::PushFont(font_opts->bold);
        else if(italic() && !bold() && *italic())
            ImGui::PushFont(font_opts->italic);
        else if(bold() && italic())
        {
            if(*bold() && !*italic())
                ImGui::PushFont(font_opts->bold);
            else if(!*bold() && *italic())
                ImGui::PushFont(font_opts->italic);
            else if(*bold() && *italic())
                ImGui::PushFont(font_opts->bold_italic);
            else
                ImGui::PushFont(font_opts->regular);
        }
        else if(bold() && !italic() && !*bold())
            ImGui::PushFont(font_opts->regular);
        else if(!bold() && italic() && !*italic())
            ImGui::PushFont(font_opts->regular);
        else if(bold() && italic() && !*bold() && !*italic())
            ImGui::PushFont(font_opts->regular);
        else
            ImGui::PushFont(font_opts->regular);

        // FIXME: Each time we call TextWrapped, it stores the location at which
        // it wants to wrap back to, which is under the starting character. We
        // actually want it under the starting character of the FIRST call to this,
        // so it wraps under the initial parent, and not under it's own start.
        if(same_line)
            ImGui::SameLine(0.0f, 0.0f);
        ImGui::TextWrapped("%s", m_text.c_str());

        for(auto comp : m_children)
            comp.draw_imgui(font_opts, true);

        if(m_color)
            ImGui::PopStyleColor();

        ImGui::PopFont();
    }
}
