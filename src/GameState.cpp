#include "GameState.hpp"

GameState::GameState(const std::string &p_lua_path, sf::RenderWindow &p_window, bool p_libs) : m_lua_state(p_libs), m_lua_script(p_lua_path, LoadScript), m_window(p_window)
{
    //Run main Lua script in our newly created state
    m_lua_state(m_lua_script.Get().c_str());

    //Populate key maps and event map
    BuildKeymap();
    BuildEventmap();

    auto lua_interface = m_lua_state["interface"];

    //Inject key-polling function into Lua state
    lua_interface["is_key_pressed"] = [&] (const std::string &p_key) -> bool
    {
        if(m_keymap.find(p_key) != m_keymap.end())
            return sf::Keyboard::isKeyPressed(m_keymap.at(p_key));
        else
            return false;
    };

    //Inject exit function into Lua state
    lua_interface["exit"] = [&] () -> void
    {
        //Closing the window terminates the main game loop
        m_window.close();
        return;
    };

    //Inject functions for loading and playing audio files
    ExposeAudio();

    return;
}

void GameState::Update(float p_dt)
{
    //Update game state in Lua
    m_lua_state["interface"]["update"](p_dt);
    
    //Pass events to Lua handler
    sf::Event event;
    while(m_window.pollEvent(event))
    {
        if(m_eventmap.find(event.type) != m_eventmap.end())
            m_eventmap.at(event.type)(event);
    }

    return;
}

void GameState::Draw()
{
    m_lua_state["interface"]["draw"]();
    return;
}

void GameState::BuildKeymap()
{
    m_keymap = {
        {"up", sf::Keyboard::Up},
        {"down", sf::Keyboard::Down},
        {"left", sf::Keyboard::Left},
        {"right", sf::Keyboard::Right}
    };

    //Build reverse table for efficient reverse lookup
    for(auto &pair : m_keymap)
        m_keymap_rev[pair.second] = pair.first;

    return;
}

void GameState::BuildEventmap()
{
    auto lua_handle_event = m_lua_state["interface"]["handle_event"];

    m_eventmap[sf::Event::Closed] = [lua_handle_event] (const sf::Event &e) -> void
    {
        lua_handle_event("closed");
        return;
    };

    m_eventmap[sf::Event::Resized] = [lua_handle_event] (const sf::Event &e) -> void
    {
        lua_handle_event("resized", e.size.width, e.size.height);
        return;
    };

    m_eventmap[sf::Event::LostFocus] = [lua_handle_event] (const sf::Event &e) -> void
    {
        lua_handle_event("lost_focus");
        return;
    };

    m_eventmap[sf::Event::GainedFocus] = [lua_handle_event] (const sf::Event &e) -> void
    {
        lua_handle_event("gained_focus");
        return;
    };

    m_eventmap[sf::Event::KeyPressed] = [this, lua_handle_event] (const sf::Event &e) -> void
    {
        sf::Keyboard::Key k = e.key.code;
        if(m_keymap_rev.find(k) != m_keymap_rev.end())
            lua_handle_event("key_pressed", m_keymap_rev.at(k));
        return;
    };

    m_eventmap[sf::Event::KeyReleased] = [this, lua_handle_event] (const sf::Event &e) -> void
    {
        sf::Keyboard::Key k = e.key.code;
        if(m_keymap_rev.find(k) != m_keymap_rev.end())
            lua_handle_event("key_released", m_keymap_rev.at(k));
        return;
    };
    
    return;
}

void GameState::ExposeAudio()
{
	auto lua_interface = m_lua_state["interface"];

    //Inject function for preloading sound buffers
    lua_interface["load_sound"] = [] (const std::string &p_path) -> void
    {
        Resource<sf::SoundBuffer>::Load(p_path, LoadSound);
        return;
    };

    //Inject function for preloading music streams
    lua_interface["load_music"] = [] (const std::string &p_path) -> void
    {
        Resource<sf::Music>::Load(p_path, LoadMusic);
        return;
    };
    
    //Inject function for playing sounds
    lua_interface["play_sound"] = [] (const std::string &p_path) -> void
    {
        Resource<sf::SoundBuffer> buffer(p_path, LoadSound);
        sf::Sound sound(buffer.Get());
        sound.play();
        return;
    };

    //Inject function for playing music
    lua_interface["play_music"] = [] (const std::string &p_path) -> void
    {
        Resource<sf::Music> music(p_path, LoadMusic);
        music.Get().play();
        return;
    };    

	return;
}

//Loads a script from a file into a std::string
std::unique_ptr<std::string> GameState::LoadScript(const std::string &p_path)
{
    auto script = std::make_unique<std::string>();
    std::ifstream file(p_path);

    if(!file)
        throw std::runtime_error("Unable to load script: " + p_path);

    std::stringstream buffer;
    buffer << file.rdbuf();
    *script = buffer.str();

    return script;
}

//Loads audio file into an sf::SoundBuffer
std::unique_ptr<sf::SoundBuffer> GameState::LoadSound(const std::string &p_path)
{
    auto sound = std::make_unique<sf::SoundBuffer>();

    if(!sound->loadFromFile(p_path))
        throw std::runtime_error("Unable to load sound: " + p_path);

    return sound;
}

//Loads a music stream into an sf::Music object
std::unique_ptr<sf::Music> GameState::LoadMusic(const std::string &p_path)
{
    auto music = std::make_unique<sf::Music>();

    if(!music->openFromFile(p_path))
        throw std::runtime_error("Unable to load music: " + p_path);

    return music;
}
