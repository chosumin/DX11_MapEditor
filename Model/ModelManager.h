#pragma once

class iObserver
{
public:
	virtual void UpdateBySubject() = 0;
};

class CommandManager;
namespace Model
{
	class ModelManager
	{
	public:
		ModelManager(CommandManager* pCommand);
		~ModelManager();

		void Update();
		void Render();
		void PostRender();
	public:
		void RegisterObserver(iObserver* observer);
		void NotifyObservers(); //�������鿡�� ����� ������ ������
	public:
		string GetJsonPath()
		{
			return jsonFilePath;
		}
		vector<class BinModel*>& GetModels()
		{
			return vecModel;
		}
	public:
		//�ݹ� �Լ�
		void LoadScene(wstring file);
		void SaveScene(wstring file);
	private:
		void GenerateModel(BinModel** model, wstring file);
		void OpenConvertDialog(wstring file = L"");
		void Convert(wstring file);

		void OpenModelDialog(wstring file = L"");
	private:
		using FuncVoid = function<void(wstring)>;
		wstring selectedFbxFile;

		vector<class BinModel*> vecModel;
		CommandManager*			o_Command;
		vector<FuncVoid>		m_vecFunc;
		iObserver*				o_pObserver;

		string jsonFilePath;
	};
}


