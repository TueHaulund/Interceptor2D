--game.lua

--Import tiny-ecs
tiny = require "scripts/tiny"

local world = tiny.world()

local systems = require "scripts/systems/systems"
local player = require "scripts/player"

local function setupWorld()
    tiny.addEntity(world, player)
    tiny.addSystem(world, systems.controlSystem)
    tiny.addSystem(world, systems.movementSystem)
    tiny.addSystem(world, systems.renderSystem)
    tiny.refresh(world)

    tiny.setSystemIndex(world, systems.controlSystem, 1)
    tiny.setSystemIndex(world, systems.movementSystem, 2)
    tiny.setSystemIndex(world, systems.renderSystem, 3)
end

local function updateWorld(dt)
    tiny.update(world, dt)
end

return {
    setupWorld = setupWorld,
    updateWorld = updateWorld
}
