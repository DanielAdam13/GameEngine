#include "ProjectNameApp.h"

#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "Components/Transform.h"
#include "Components/TextComponent.h"
#include "Components/FPSComponent.h"

#include "Font.h"
#include "ResourceManager.h"

#include "Renderer.h"
#include "Services/ServiceLocator.h"
#include "Services/InputManager.h"
#include "Commands/ChangeWindowSizeCommand.h"

#include <memory>
#include <utility>

void customProject::ProjectNameApp::Load()
{
	ge::Renderer::GetInstance().SetWindowSize({ 800, 800 });

	// 1. Scene setup - a project needs at least one scene to hold its GameObjects
	ge::Scene& scene{ ge::SceneManager::GetInstance().CreateScene("MainScene") };
	ge::SceneManager::GetInstance().SwitchToSceneWithName("MainScene");

	// 2. An FPS Counter GameObject with a Transform (automatic, every game object has one)
	// 
	// FPSComponent does not draw anything itself (its RenderComponent method is a no-op)
	// - it just meaasures framme time and writes the result into a "sibling" TextComponent 
	// on the same GameObject.
	// 
	// TextComponent needs a loaded Font. Feel free to drop any true type dont into the resources/ folder...
	ge::Font* font{ ge::ResourceManager::GetInstance().LoadFont("YourFont.otf", 24) };

	auto fpsCounter{ std::make_unique<ge::GameObject>("FPSCounterGO") };
	// Center it using the engine's design resolution so it is centered regardless of window size
	const auto [designWidth, designHeight] { ge::Renderer::GetInstance().GetWindowDesignSize() };
	fpsCounter->GetComponent<ge::Transform>()->SetLocalPosition(designWidth * 0.5f, designHeight * 0.5f);

	fpsCounter->AddComponent<ge::TextComponent>(fpsCounter.get(), "-- FPS", font, SDL_Color{ 255, 255, 255, 255 });
	fpsCounter->AddComponent<ge::FPSComponent>(fpsCounter.get());
	
	scene.Add(std::move(fpsCounter));

	// 3. Bind an existing engine command to a key.
	BindInputCommands();
}

void customProject::ProjectNameApp::Update(float)
{
}

void customProject::ProjectNameApp::FixedUpdate(float)
{
}

void customProject::ProjectNameApp::BindInputCommands()
{
	auto& input{ ge::ServiceLocator::GetInputManager() };

	// F1: toggle between current window size and 1200x1200
	input.BindKeyboardCommand(SDL_SCANCODE_F1,
		ge::InputManager::InputTrigger::Pressed,
		std::make_unique<ge::ChangeWindowSizeCommand>(1200, 1200));
}
