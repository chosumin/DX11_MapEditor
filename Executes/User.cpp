#include "stdafx.h"
#include "User.h"
#include "./Model/ModelManager.h"
#include "./Model/BinModel.h"
#include "./Model/MeshPart.h"
#include "./Command/Command.h"
#include "./Command/Commands.h"
#include "./Utilities/Path.h"
#include "./Utilities/String.h"

User::User(ExecuteValues* values, Model::ModelManager* modelM
		   , CommandManager* manager, vector<Model::BinModel*>& vecModel)
	:Execute(values)
	, o_vecModel(&vecModel)
	, o_pCommandManager(manager)
{
	modelM->RegisterObserver(this);
}

User::~User()
{
}

void User::Update()
{
	//컨트롤 누를시 다중 선택
	bool isMultiPick = false;
	if (Keyboard::Get()->Press(VK_CONTROL))
		isMultiPick = true;

	//마우스 클릭하면 피킹 검사
	if (Mouse::Get()->Down(0))
		PickModels(isMultiPick);

	//피킹 모델 위치 이동
	TranslateModels();

	for (auto& mesh : m_vecPickedModel)
	{
		if (mesh)
			mesh->Update();
	}

	//실행 취소, 재실행
	UndoRedo();

	UpdateVecPickedModel();
}

void User::PostRender()
{
	ImGui::Begin("Transform");
	{
		if (m_vecPickedModel.empty() == false)
		{
			auto firstModel = m_vecPickedModel.front();

			//SRT 변환
			SRTPickedMeshes("Transform", 0);
			SRTPickedMeshes("Scaling", 1);
			SRTPickedMeshes("Rotation", 2);

			//색상 변환
			if (ImGui::ColorEdit4("Diffuse", firstModel->GetDiffuse()))
			{
				for (auto& model : m_vecPickedModel)
					model->GetDiffuse() = firstModel->GetDiffuse();
			}
		}//if(m_vecPickedModel)
	}
	ImGui::End();
}

vector<Model::BinModel*>& User::GetModels()
{
	return m_vecPickedModel;
}

void User::UpdateVecPickedModel()
{
	m_vecPickedModel.clear();

	for (auto& model : *o_vecModel)
	{
		if (model->GetPickingState())
			m_vecPickedModel.push_back(model);
	}
}

void User::PickModels(bool bMultiPick)
{
	//마우스 위치, 방향 얻어옴
	D3DXVECTOR3 start;
	values->MainCamera->GetPosition(&start);
	D3DXVECTOR3 direction;
	direction = values->MainCamera->GetDirection(values->Viewport
												 , values->Perspective);

	//모델 순회하며 피킹된 것 찾음
	for (auto model : *o_vecModel)
	{
		auto mesh = model->GetIntersectedMesh(start, direction);
		if (mesh)
		{
			if (bMultiPick == false)
				ResetPicked();

			o_pCommandManager->Execute(new SelectingCommand(this, model));

			return;
		}
	}
}

void User::ResetPicked()
{
	for (auto& model : m_vecPickedModel)
	{
		if (model)
			model->SetPickingState(false);
	}
	m_vecPickedModel.clear();
}

void User::TranslateModels()
{
	static bool bDown = false;

	bool* press = GetDirection();

	for (auto& model : m_vecPickedModel)
	{
		if (model == nullptr)
			continue;
		if (press[0] || press[1] || press[2] || press[3] || press[4] || press[5])
		{
			if (!bDown)
			{
				bDown = true;
				m_vecPos = model->GetVec3(0);
			}
			//전후 이동
			MoveModel(model->GetVec3(0).z, press, 0, 1);
			//좌우 이동
			MoveModel(model->GetVec3(0).x, press, 2, 3);
			//상하 이동
			MoveModel(model->GetVec3(0).y, press, 4, 5);
		}
	}

	if (bDown == true)
	{
		if (!press[0] && !press[1] && !press[2] && !press[3] && !press[4] && !press[5])
		{
			InputPosCommand();
			bDown = false;
		}
	}

	SAFE_DELETE_ARRAY(press);
}

void User::UndoRedo()
{
	if (Keyboard::Get()->Press(VK_CONTROL))
	{
		if (Keyboard::Get()->Down('Z'))
			o_pCommandManager->Undo();
		else if (Keyboard::Get()->Down('Y'))
			o_pCommandManager->Redo();
	}
}

bool * User::GetDirection()
{
	bool* press = new bool[6]{ 0, };
	if (Keyboard::Get()->Press(VK_UP))
		press[0] = true;
	else if (Keyboard::Get()->Press(VK_DOWN))
		press[1] = true;

	if (Keyboard::Get()->Press(VK_RIGHT))
		press[2] = true;
	else if (Keyboard::Get()->Press(VK_LEFT))
		press[3] = true;

	if (Keyboard::Get()->Press('K'))
		press[4] = true;
	else if (Keyboard::Get()->Press('M'))
		press[5] = true;

	return press;
}

void User::MoveModel(float& originPos, bool press[], int a, int b)
{
	originPos += press[a] * Time::Delta() * 25;
	originPos -= press[b] * Time::Delta() * 25;
}

void User::InputPosCommand()
{
	auto firstModel = m_vecPickedModel[0];
	auto posGap = firstModel->GetVec3(0) - m_vecPos;

	o_pCommandManager->Execute(new PosCommand(m_vecPickedModel, posGap));
}

void User::SRTPickedMeshes(string typeStr, UINT type)
{
	if (ImGui::InputFloat3(typeStr.c_str(), m_vecPickedModel[0]->GetVec3(type), 3))
	{
		for (auto& model : m_vecPickedModel)
			model->GetVec3(type) = m_vecPickedModel[0]->GetVec3(type);
	}
}

void User::UpdateBySubject()
{
	m_vecPickedModel.clear();
}

void User::PreRender()
{
}

void User::Render()
{
}

void User::ResizeScreen()
{
}
