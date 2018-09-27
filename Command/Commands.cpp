#include "stdafx.h"
#include "Commands.h"
#include "./Executes/User.h"
#include "./Model/ModelManager.h"
#include "./Model/FbxModel.h"
#include "./Model/BinModel.h"
#include "./Model/MeshPart.h"

Command * SelectingCommand::Clone()
{
	return new SelectingCommand(*this);
}

void SelectingCommand::Execute()
{
	if (o_pPickedModel)
		o_pPickedModel->SetPickingState(true);
}

void SelectingCommand::Undo()
{
	if(o_pPickedModel)
		o_pPickedModel->SetPickingState(false);
}

void SelectingCommand::Redo()
{
	Execute();
}

/***************************************************************************/

Command * PosCommand::Clone()
{
	return nullptr;
}

void PosCommand::Execute()
{
	//do nothing
}

void PosCommand::Undo()
{
	for (auto& mesh : m_vecModel)
		mesh->GetVec3(0) -= m_vecPos;
}

void PosCommand::Redo()
{
	for (auto& mesh : m_vecModel)
		mesh->GetVec3(0) += m_vecPos;
}

/*******************************************************************/