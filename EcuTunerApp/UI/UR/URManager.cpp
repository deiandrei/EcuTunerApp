#include "URManager.h"

URManager* URManager::m_instance = nullptr;

URManager::URManager() : QObject()
{
}

URManager::~URManager()
{
}

URManager* URManager::Instance() {
	if (!m_instance) {
		m_instance = new URManager();
	}

	return m_instance;
}

void URManager::PushCommand(IURCommand* command) {
	m_undoCommands.push_back(command);

	if (m_undoCommands.length() == 1) emit EnableUndo();
}

void URManager::Undo() {
	if (m_undoCommands.isEmpty()) return;

	m_undoCommands.front()->Undo();
	m_undoCommands.pop_front();

	if (m_undoCommands.isEmpty()) {
		emit DisableUndo();
	}
}
