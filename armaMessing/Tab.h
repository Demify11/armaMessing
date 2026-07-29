#pragma once
#include <string>
#include "Group.h"

class Tab
{
protected:
	std::string m_Icon;
	std::string m_Name;

	std::vector<Group> m_Groups;

public:
	Tab(std::string icon, std::string name)
		: m_Icon(std::move(icon)), m_Name(std::move(name)) {} //We can initialize stuff before main body

	Group& AddGroup(std::string name);

	void DrawPage();

	const std::string& Icon() const {
		return m_Icon;
	}

};

