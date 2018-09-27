#pragma once
#include "Command.h"

class SelectingCommand : public Command
{
public:
	SelectingCommand(User* user, Model::BinModel* pPickedModel) 
		: m_pUser(user), o_pPickedModel(pPickedModel)
	{
		m_sMessage = "SelectMesh";
	}
	~SelectingCommand() {}

	// Command을(를) 통해 상속됨
	virtual Command *	Clone() override;
	virtual void		Execute() override;
	virtual void		Undo() override;
	virtual void		Redo() override;
private:
	User*				m_pUser;
	Model::BinModel*	o_pPickedModel;
};

/******************************************************************/

class PosCommand : public Command
{
public:
	PosCommand(vector<Model::BinModel*>& vecModel, D3DXVECTOR3 pos)
		: m_vecModel(vecModel), m_vecPos(pos)
	{
		m_sMessage = "SRTC";
	}
	~PosCommand() {}

	// Command을(를) 통해 상속됨
	virtual Command *	Clone() override;
	virtual void		Execute() override;
	virtual void		Undo() override;
	virtual void		Redo() override;
private:
	vector<Model::BinModel*>m_vecModel;
	D3DXVECTOR3				m_vecPos;
};

/******************************************************************/