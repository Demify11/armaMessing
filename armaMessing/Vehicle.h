#pragma once

class Vehicle: public Entity
{
public:


	Vector3 VehVelocity;

	Vector3 VehHeadPos;
	Vector3 m_TransformedHeadPos;

	std::string CarName = "";


	Entity* m_Driver = nullptr;	// null if there's none!!!!
	std::vector<Entity*> m_Passengers;	// same heree ^^

	void Cache(bool State) override;

private:


	void CacheCleanName(bool State);
	void CacheDriver(bool State);

	Entity* GetTargetInVehicleTransform();

protected:
	void OnClassify() override;

	virtual Vector3 GetHeadPos() override;
	virtual Vector3 GetVelocity() override;

};

