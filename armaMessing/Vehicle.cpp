#include "Framework.h"



void Vehicle::Cache(bool State) {

	if (!m_Base)
		return;

	Entity::Cache(State);
	CacheCleanName(State);
	CacheDriver(State);
	GetTargetInVehicleTransform();
}

void Vehicle::CacheCleanName(bool State) {

	if (State) {

		auto EntityType = Coms->ReadVirtual<UINT64>(m_Base + 0x150);

		if (!EntityType)
			return;

		auto CleanNameEntry = Coms->ReadVirtual<UINT64>(EntityType + 0x13B8);

		auto CleanNameSize = Coms->ReadVirtual<INT32>(CleanNameEntry + 0x8);

		CarName = Coms->ReadString(CleanNameEntry + 0x10);
	}
}

void Vehicle::CacheDriver(bool State) {

	if (State) {

		auto Driver = Coms->ReadVirtual<UINT64>(m_Base + 0xE78); //Was D70, I think its acutally...

		if (Driver == 0x0) {
			
			type = EntityType::Player;
			return;
		}

		m_Driver->m_Base = Driver;

	}

	m_Driver->Cache(State);
}

Entity* Vehicle::GetTargetInVehicleTransform() { //in future make part of caching

	// What type of vehicle am i?

	if (m_Driver->m_Base == 0)
		return NULL;

	/* only works for driver.*/
	auto it = g_HeadLookup.find(CarName);
	if (it == g_HeadLookup.end())
		return m_Driver;

	//auto ModelPos = g_HeadLookup.at(CarName);
	Vector3 ModelPos = it->second;

	if (!VisualState)
		return NULL;

	/*
	// resolve HeadPos
	// - do matrix multiplication with the visualstate transform
	//auto VisualState = Coms->ReadVirtual<UINT64>(m_Base + 0x180);
	//auto VisualState = VehVisualState;




	//auto Page = Coms->ReadVirtual<PageRead>(VisualState + 0x8);

	*
	*
	* float* Transform = new float[12];
	*
	*(Vector3*)&Transform[0] = Coms->ReadVirtual<Vector3>(VisualState + 0x8 + sizeof(Vector3) * 0);
	*(Vector3*)&Transform[3] = Coms->ReadVirtual<Vector3>(VisualState + 0x8 + sizeof(Vector3) * 1);
	*(Vector3*)&Transform[6] = Coms->ReadVirtual<Vector3>(VisualState + 0x8 + sizeof(Vector3) * 2);
	*(Vector3*)&Transform[9] = Coms->ReadVirtual<Vector3>(VisualState + 0x8 + sizeof(Vector3) * 3);

	float _x = ModelPos.x * Transform[0] + ModelPos.y * Transform[3] + ModelPos.z * Transform[6] + Transform[9];
	float _y = ModelPos.x * Transform[1] + ModelPos.y * Transform[4] + ModelPos.z * Transform[7] + Transform[10];
	float _z = ModelPos.x * Transform[2] + ModelPos.y * Transform[5] + ModelPos.z * Transform[8] + Transform[11];



	delete[] Transform;*/
	float _x = ModelPos.x * VisualCamera.right.x + ModelPos.y * VisualCamera.up.x + ModelPos.z * VisualCamera.forward.x + VisualCamera.pos.x;
							
	float _y = ModelPos.x * VisualCamera.right.y + ModelPos.y * VisualCamera.up.y + ModelPos.z * VisualCamera.forward.y + VisualCamera.pos.y;
							
	float _z = ModelPos.x * VisualCamera.right.z + ModelPos.y * VisualCamera.up.z + ModelPos.z * VisualCamera.forward.z + VisualCamera.pos.z;

	m_TransformedHeadPos = Vector3(_x, _y, _z);

	return m_Driver;


	return m_Driver;
}

Vector3 Vehicle::GetHeadPos()
{
	return m_TransformedHeadPos;
}

Vector3 Vehicle::GetVelocity()
{
	return m_Velocity;
}

void Vehicle::OnClassify() {
	m_Driver = new Entity();
}