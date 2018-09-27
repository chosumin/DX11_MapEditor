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
	// Execute��(��) ���� ��ӵ�
	virtual void PreRender() override;
	virtual void Update() override;
	virtual void Render() override;
	virtual void ResizeScreen() override;
	virtual void PostRender() override;
public:
	vector<Model::BinModel*>& GetModels();
	void PickModels(bool bMultiPick); //�� ��ŷ
private:
	// iObserver��(��) ���� ��ӵ�
	virtual void UpdateBySubject() override;
private:
	//��ŷ �� ����
	void UpdateVecPickedModel();
	void ResetPicked();
	//��ŷ�� �𵨵��� ��ġ ��ȯ
	void TranslateModels();
	//���� ���, �����
	void UndoRedo();

	bool* GetDirection();
	void MoveModel(float& originPos, bool press[], int a, int b);
	void InputPosCommand();
	void SRTPickedMeshes(string typeStr, UINT type);
private:
	vector<Model::BinModel*>*	o_vecModel; //�ܺο��� ���� ��� ��
	vector<Model::BinModel*>	m_vecPickedModel; //��ŷ�� �޽�
	CommandManager*				o_pCommandManager; //�Է� �Ŵ���

	D3DXVECTOR3		m_vecPos;


};

