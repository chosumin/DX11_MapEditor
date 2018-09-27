#pragma once

class User;
namespace Model 
{
	class FbxModelManager;
	class FbxModel;
	class BinModel;
	class MeshPart;
}

class Command
{
public:
	void Destroy()
	{
		if (m_pNext)
			m_pNext->Destroy();
		delete this;
	}
	string GetCommandMessage() const { return m_sMessage; }
protected:
	Command() :m_pPrev(nullptr), m_pNext(nullptr) {}
	virtual ~Command() {}
	
	virtual Command*	Clone() = 0;
	virtual void		Execute() = 0;
	virtual void		Undo() = 0;
	virtual void		Redo() = 0;
protected:
	Command*	m_pPrev;
	Command*	m_pNext;
	string		m_sMessage;
private:
	friend class CommandManager;
};

class HeaderCommand : public Command
{
public:
	HeaderCommand() { m_sMessage = ""; }
	// Command을(를) 통해 상속됨
	virtual Command * Clone() override { return nullptr; }
	virtual void Execute() override {}
	virtual void Undo() override {}
	virtual void Redo() override {}
};

enum class CommandType
{
	Generate, Delete, Select, Position
};

class CommandManager
{
public:
	CommandManager() 
		:m_pHeader(new HeaderCommand()), m_pCurCommand(m_pHeader) {}
	~CommandManager() 
	{ 
		Destroy(m_pHeader); 
		for (auto& command : m_mapCommand)
			SAFE_DELETE(command.second);

		SAFE_DELETE(m_pHeader);
	}

	bool Press(CommandType eCommandType);
	void Execute(Command* pCommand);
	void Undo();
	void Redo();
	void Destroy(Command* pStartPoint = nullptr);
private:
	Command*	m_pHeader;
	Command*	m_pCurCommand;

	map<CommandType, Command*> m_mapCommand;
};