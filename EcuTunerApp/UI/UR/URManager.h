#ifndef URMANAGER_H
#define	URMANAGER_H

#include <qlist.h>
#include <qobject.h>

class IURCommand {
	public:
		virtual void Undo() = 0;
		//virtual void Redo() = 0;
};

class URManager : public QObject {
	Q_OBJECT

	public:
		URManager(URManager& other) = delete;
		void operator=(const URManager&) = delete;

		static URManager* Instance();

		void PushCommand(IURCommand* command);
		void Undo();
		//void Redo();

	signals:
		void EnableUndo();
		void DisableUndo();

	private:
		URManager();
		~URManager();

		static URManager* m_instance;

	private:
		QList<IURCommand*> m_undoCommands;
		//QList<IURCommand*> m_RedoCommands;


};

#endif
