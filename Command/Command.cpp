#include "stdafx.h"
#include "Command.h"
#include "Commands.h"

//�� ���� Ŀ�ǵ�
//�� ���� Ŀ�ǵ�
//TODO : SRT, ���� ���� Ŀ�ǵ�
//TODO : �� ���� Ŀ�ǵ�
//TODO : �ҷ�����, ����ÿ� ��� Ŀ�ǵ� �����

bool CommandManager::Press(CommandType eCommandType)
{
	return false;
}

void CommandManager::Execute(Command* pCommand)
{
	//����� 0�� ���
	if (m_pHeader == m_pCurCommand)
	{
		m_pHeader->m_pNext = pCommand;
		pCommand->m_pPrev = m_pHeader;
		m_pCurCommand = pCommand;
	}
	else
	{
		//Undo Ŀ�ǵ尡 ���� ���, ��� ��� ����
		if (m_pCurCommand->m_pNext)
			m_pCurCommand->m_pNext->Destroy();

		//������ ��忡 ����
		m_pCurCommand->m_pNext = pCommand;
		pCommand->m_pPrev = m_pCurCommand;
		m_pCurCommand = pCommand;
	}
	
	//�Է� ����
	pCommand->Execute();
}

void CommandManager::Undo()
{
	//����� 0�̸� ���� ����
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
	//Undo�Է� �������� ���� ����
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