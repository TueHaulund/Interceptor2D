#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include <string>
#include <map>
#include <stdexcept>
#include <memory>
#include <fstream>
#include <sstream>
#include <functional>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <selene.h>

#include "Resource.hpp"

class GameState
{
    public:
        GameState(const std::string &p_lua_path, sf::RenderWindow &p_window, bool p_libs = true);
        void Update(float p_dt);
        void Draw();

    private:
        sel::State m_lua_state;
        Resource<std::string> m_lua_script;
        
        sf::RenderWindow &m_window;       
        std::map<std::string, sf::Keyboard::Key> m_keymap;
        std::map<sf::Keyboard::Key, std::string> m_keymap_rev;
        std::map<sf::Event::EventType, std::function<void(const sf::Event&)>> m_eventmap;

        void BuildKeymap();
        void BuildEventmap();
        void ExposeAudio();
        
        static std::unique_ptr<std::string> LoadScript(const std::string &p_path);
        static std::unique_ptr<sf::SoundBuffer> LoadSound(const std::string &p_path);
        static std::unique_ptr<sf::Music> LoadMusic(const std::string &p_path);
};

#endif
