#include "RenderState.hpp"

void RenderState::operator()(sel::State &p_lua_state, sf::RenderWindow &p_window)
{
    WindowInterface(p_lua_state, p_window);
    SpriteInterface(p_lua_state, p_window);
    return;
}

void RenderState::WindowInterface(sel::State &p_lua_state, sf::RenderWindow &p_window)
{
    auto lua_interface = p_lua_state["interface"];
    
    lua_interface["create_window"] = [&p_window] (int p_w, int p_h, int p_bpp, int p_fps, const std::string &p_title) -> void
    {
        p_window.create(sf::VideoMode(p_w, p_h, p_bpp), p_title, sf::Style::None);
        p_window.setFramerateLimit(p_fps);
        p_window.setKeyRepeatEnabled(false);
        return;
    };

    lua_interface["resize_window"] = [&p_window] (int p_w, int p_h) -> void
    {
        p_window.setSize(sf::Vector2u(p_w, p_h));
        return;
    };

    return;
}

void RenderState::SpriteInterface(sel::State &p_lua_state, sf::RenderWindow &p_window)
{
    auto lua_interface = p_lua_state["interface"];
    auto map_apply = [this] (const std::string &p_identifier, std::function<void(sf::Sprite&)> p_func) -> bool
    {
        if(m_spritemap.find(p_identifier) != m_spritemap.end())
        {
           p_func(*m_spritemap.at(p_identifier));
           return true;
        }

        return false;
    };

    lua_interface["load_sprite"] = [this, map_apply] (const std::string &p_identifier, const std::string &p_path) -> void
    {
        Resource<sf::Texture> texture(p_path, LoadTexture);

        bool reload = map_apply(p_identifier, [texture] (sf::Sprite &s) -> void
        {
            s.setTexture(texture.Get());
            return;
        });

        if(!reload)
            m_spritemap[p_identifier] = std::make_unique<sf::Sprite>(texture.Get());

        return;
    };

    lua_interface["clear_sprite"] = [this] (const std::string &p_identifier) -> void
    {
        m_spritemap.erase(p_identifier);
        return;
    };

    //TODO: Change type of p_x and p_y to lua_Number when Selene regressions have been fixed
    lua_interface["set_sprite_position"] = [map_apply] (const std::string &p_identifier, int p_x, int p_y) -> void
    {
        map_apply(p_identifier, [p_x, p_y] (sf::Sprite &s) -> void
        {
            s.setPosition(static_cast<float>(p_x), static_cast<float>(p_y));
            return;
        });
        return;
    };

    lua_interface["set_sprite_clip"] = [map_apply] (const std::string &p_identifier, int p_l, int p_t, int p_w, int p_h) -> void
    {
        map_apply(p_identifier, [p_l, p_t, p_w, p_h] (sf::Sprite &s) -> void
        {
            s.setTextureRect(sf::IntRect(p_l, p_t, p_w, p_h));
            return;
        });
        return;
    };

    lua_interface["draw_sprite"] = [map_apply, &p_window] (const std::string &p_identifier) -> void
    {
        map_apply(p_identifier, [&p_window] (sf::Sprite &s) -> void
        {
            p_window.draw(s);
            return;
        });
        return;
    };

    return;
}

std::unique_ptr<sf::Texture> RenderState::LoadTexture(const std::string &p_path)
{
    auto texture = std::make_unique<sf::Texture>();

    if(!texture->loadFromFile(p_path))
        throw std::runtime_error("Unable to load texture: " + p_path);

    return texture;
}
