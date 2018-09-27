#include "stdafx.h"
#include "Command.h"
#include "Commands.h"

//모델 생성 커맨드
//모델 삭제 커맨드
//TODO : SRT, 색상 변경 커맨드
//TODO : 모델 선택 커맨드
//TODO : 불러오기, 저장시에 모든 커맨드 지우기

bool CommandManager::Press(CommandType eCommandType)
{
	return false;
}

void CommandManager::Execute(Command* pCommand)
{
	//사이즈가 0인 경우
	if (m_pHeader == m_pCurCommand)
	{
		m_pHeader->m_pNext = pCommand;
		pCommand->m_pPrev = m_pHeader;
		m_pCurCommand = pCommand;
	}
	else
	{
		//Undo 커맨드가 있을 경우, 노드 모두 삭제
		if (m_pCurCommand->m_pNext)
			m_pCurCommand->m_pNext->Destroy();

		//마지막 노드에 연결
		m_pCurCommand->m_pNext = pCommand;
		pCommand->m_pPrev = m_pCurCommand;
		m_pCurCommand = pCommand;
	}
	
	//입력 실행
	pCommand->Execute();
}

void CommandManager::Undo()
{
	//사이즈가 0이면 실행 안함
	if (m_pHeader == m_pCurCommand)
		return;

	if (m_pCurCommand)
	{
		m_pCurCommand->Undo();
		m_pCurCommand = m_pCurCommand->m_pPrev;
	}
}

void CommandManager::Redo()
{
	//Undo입력 없었으면 실행 안함
	if (m_pCurCommand->m_pNext == nullptr)
		return;

	auto next = m_pCurCommand->m_pNext;
	if (next)
	{
		next->Redo();
		m_pCurCommand = next;
	}
}

void CommandManager::Destroy(Command* pStartPoint)
{
	if (pStartPoint == nullptr)
	{
		if(m_pHeader->m_pNext)
			m_pHeader->m_pNext->Destroy();
		m_pCurCommand = m_pHeader;
	}
	else
		pStartPoint->m_pNext->Destroy();
}