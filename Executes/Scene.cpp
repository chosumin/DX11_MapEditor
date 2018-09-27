#include "stdafx.h"
#include "Scene.h"
#include "./Model/ModelManager.h"
#include "./Command/Command.h"
#include "./Executes/User.h"
#include "./Landscape/Terrain.h"
#include "./Utilities/String.h"
#include "./Utilities/Path.h"
#include "./JsonParser/JsonParser.h"

Scene::Scene(ExecuteValues * values)
	:Execute(values)
	, mode(0)
{
	commandManager = new CommandManager();
	manager = new Model::ModelManager(commandManager);
	user = new User(values, manager, commandManager, manager->GetModels());

	terrain = new Landscape::Terrain(values);
}

Scene::~Scene()
{
	SAFE_DELETE(manager);
	SAFE_DELETE(commandManager);
	SAFE_DELETE(user);
	SAFE_DELETE(terrain);
}

void Scene::Update()
{
	manager->Update();
	
	if (mode == 0)
		terrain->Update();
	else
		user->Update();
}

void Scene::Render()
{
	manager->Render();
	user->Render();
	terrain->Render();
}

void Scene::PostRender()
{
	ImGui::Begin("Insertion Mode");
	{
		ImGui::RadioButton("Terrain", &mode, 0);
		ImGui::RadioButton("Model", &mode, 1);
	}
	ImGui::End();

	manager->PostRender();

	if(mode == 0)
		terrain->PostRender();
	else
		user->PostRender();

	ImGuiSceneMenu();
}

void Scene::ImGuiSceneMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Scene"))
		{
			//json ����
			if (ImGui::MenuItem("Save", ""))
			{
				Path::SaveFileDialog(L"Save Scene", L"Json file\0*.json", Contents, [this](wstring file)
				{
					Json::Value root;

					//�� json���ϸ� ����
					Json::Value model = manager->GetJsonPath();
					root["ModelFile"] = model;

					//���� json���ϸ� ����
					Json::Value jTerrain;
					terrain->WriteOnJson(&jTerrain);
					root["Terrain"] = jTerrain;

					auto tempStr = String::WStringToString(file);
					JsonParser::Get()->WriteOnFile(tempStr, &root);
				});
			}
			if (ImGui::MenuItem("Load", ""))
			{
				Path::OpenFileDialog(L"Save Scene", L"Json file\0*.json", Contents, [this](wstring file)
				{
					Json::Value root;
					auto tempStr = String::WStringToString(file);
					JsonParser::Get()->ParseValue(tempStr, &root);

					//�� json���ϸ� �ҷ�����
					string model = root["ModelFile"].asString();

					auto tempWstr = String::StringToWString(model);
					manager->LoadScene(tempWstr);

					//���� json���ϸ� �ҷ�����
					auto jTerrain = root["Terrain"];
					terrain->ReadJson(jTerrain);
				});
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

