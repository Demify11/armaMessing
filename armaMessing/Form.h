#pragma once
#include "Tab.h"
class Form
{
protected:
	int SizeX = 600;
	int SizeY = 400;
	int m_Selected = 0;

	std::vector<Tab> m_Tabs;
public:
	Tab& AddTab(std::string icon, std::string name);
	void Init();
	void Draw();


};

