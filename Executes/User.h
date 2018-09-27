#pragma once
#include "./Model/ModelManager.h"

class CommandManager;
namespace Model 
{ 
	class BinModel;
	class MeshPart;
}

class User : public Execute, public iObserver
{
public:
	User(ExecuteValues* values, Model::ModelManager* modelM, CommandManager* manager
		 , vector<Model::BinModel*>& vecModel);
	~User();
public:
	// Execute을(를) 통해 상속됨
	virtual void PreRender() override;
	virtual void Update() override;
	virtual void Render() override;
	virtual void ResizeScreen() override;
	virtual void PostRender() override;
public:
	vector<Model::BinModel*>& GetModels();
	void PickModels(bool bMultiPick); //모델 피킹
private:
	// iObserver을(를) 통해 상속됨
	virtual void UpdateBySubject() override;
private:
	//피킹 모델 갱신
	void UpdateVecPickedModel();
	void ResetPicked();
	//피킹된 모델들의 위치 변환
	void TranslateModels();
	//실행 취소, 재실행
	void UndoRedo();

	bool* GetDirection();
	void MoveModel(float& originPos, bool press[], int a, int b);
	void InputPosCommand();
	void SRTPickedMeshes(string typeStr, UINT type);
private:
	vector<Model::BinModel*>*	o_vecModel; //외부에서 받은 모든 모델
	vector<Model::BinModel*>	m_vecPickedModel; //피킹된 메쉬
	CommandManager*				o_pCommandManager; //입력 매니저

	D3DXVECTOR3		m_vecPos;


};

