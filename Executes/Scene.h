#pragma once

class User;
class CommandManager;
namespace Model { class ModelManager; }
namespace Landscape { class Terrain; }
class Scene : public Execute
{
public:
	Scene(ExecuteValues* values);
	~Scene();
public:
	// Execute을(를) 통해 상속됨
	virtual void Update() override;
	virtual void PreRender() override {}
	virtual void Render() override;
	virtual void PostRender() override;
	virtual void ResizeScreen() override {}
private:
	void ImGuiSceneMenu();
private:
	Model::ModelManager*	manager;
	CommandManager*			commandManager;
	User*					user;
	Landscape::Terrain*		terrain;
	
	int mode;
};