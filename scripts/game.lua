--game.lua

local options = require "scripts/options"
local screenCenter = {
    x = options.video.w / 2,
    y = options.video.h / 2
}

local tiny = require "scripts/tiny"
local gameWorld = tiny.world()

local isPaused = false

local function setupGame()
    --Semi-transparent rectangle overlay
    interface.createRectangle("pauseOverlay", options.video.w, options.video.h)
    interface.setShapeFillColor("pauseOverlay", 0, 0, 0, 128)
    interface.setShapePosition("pauseOverlay", 0, 0)

    interface.loadSprite("pauseBox", "data/sprites/pausebox.tga")
    interface.setSpritePosition("pauseBox", screenCenter.x - 200, screenCenter.y - 100)
end

local function startGame(level)
    isPaused = false

    local systems = require "scripts/systems/systems"

    --Create and register systems
    for _, createSystem in pairs(systems) do
        tiny.addSystem(gameWorld, createSystem())
    end

    tiny.refresh(gameWorld)

    --Test explosion
    tiny.addEntity(gameWorld, {
        position = {
            x = 200,
            y = 200
        },

        lifetime = 0.5,

        emitter = {
            sources = {
                {
                    rate = 200,
                    lifetime = {0.1, 1.0, 0.1},
                    rotate = true,

                    offset = {
                        x = 0,
                        y = 0
                    },

                    size = {
                        w = {1, 2},
                        h = {1, 5}
                    },

                    velocity = {
                        x = {-100, 100},
                        y = {-100, 100}
                    },

                    color = {
                        r = 255,
                        g = {50, 255},
                        b = 0,
                        a = 255
                    }
                }
            }
        }
    })
end

local function stopGame()
    tiny.clearEntities(gameWorld)
    tiny.clearSystems(gameWorld)
end

local function isRenderSystem(_, system)
    return system._isRender
end

local function update(dt)
    if isPaused then
        tiny.update(gameWorld, dt, isRenderSystem)
        interface.drawShape("pauseOverlay")
        interface.drawSprite("pauseBox")
    else
        tiny.update(gameWorld, dt)
    end
end

local function input(k, exitToMenu)
    if k == "p" then
        isPaused = not isPaused
    elseif k == "escape" then
        exitToMenu()
    end
end

return {
    setupGame = setupGame,
    startGame = startGame,
    stopGame = stopGame,
    update = update,
    input = input
}
