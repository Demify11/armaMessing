#pragma once
#include <vector>
#include "Element.h"
class Group
{
	std::string m_Name;
	std::vector<Element*> m_Elements;
    float m_Height = 200.0f;
    float m_Provisional = 200.0f;
    float m_Measured = 0.0f;

public:

    explicit Group(std::string name) : m_Name(std::move(name)) {}

    ~Group() {
        for (auto* el : m_Elements)
            delete el;
    }

    void SetHeight(float h) { m_Height = h; }
    float GetHeight() const { return m_Height; }
    float Height();

    template <typename T, typename... Args>
    T& Add(Args&&... args) {
        T* e = new T(std::forward<Args>(args)...);
        m_Elements.push_back(e);
        return *e;;
    }

	void Draw(float width);


};

