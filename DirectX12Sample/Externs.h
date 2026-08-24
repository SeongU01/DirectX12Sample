#pragma once

namespace Global
{
	extern Device*      device;
	extern Renderer*    renderer;
	extern ViewManager* viewManager;
    extern CommandController* commandController;
    extern ResourceManager*   resourceManager;
    extern bool               isRayTracing;
}
