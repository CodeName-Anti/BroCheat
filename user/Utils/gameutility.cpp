#include <includes.h>
#include <helpers.h>

using namespace app;

void DisableAntiCheat()
{
	InjectionDetector_StopDetection(nullptr);
	ObscuredCheatingDetector_StopDetection(nullptr);
	SpeedHackDetector_StopDetection(nullptr);
	TimeCheatingDetector_StopDetection(nullptr);
	WallHackDetector_StopDetection(nullptr);

	InjectionDetector_Dispose(nullptr);
	ObscuredCheatingDetector_Dispose(nullptr);
	SpeedHackDetector_Dispose(nullptr);
	TimeCheatingDetector_Dispose(nullptr);
	WallHackDetector_Dispose(nullptr);
}

app::PlayerSetup* GetLocalPlayer()
{
	auto networkManager = (*NetworkManager__TypeInfo)->static_fields->instance;

	if (networkManager != nullptr)
	{
		return networkManager->fields.myPS;
	}

	return nullptr;
}

bool WasPressedThisFrame(KeyControl* key)
{
	return ButtonControl_get_wasPressedThisFrame((ButtonControl*)key, nullptr);
}

bool IsPressed(KeyControl* key)
{
	return ButtonControl_get_isPressed((ButtonControl*)key, nullptr);
}

string FormatPlayername(PlayerSetup* player)
{
	string name = il2cppi_to_string(player->fields.clothManager->fields.playerName);

	name = name.substr(0, name.find("|"));

	if (player->fields.isMine)
		name += "(Self)";

	if (player->fields.isBot)
		name += "(Bot)";

	if (player->fields.isBroccoli)
		name += "(Broccoli)";

	if (player->fields.isSpectator)
		name += "(Spectator)";

	return name;
}

app::Vector3 ProjectOnPlane(Vector3 vector, Vector3 planeNormal)
{
	float num = Vector3_Dot(planeNormal, planeNormal, nullptr);
	if (num < (*Mathf__TypeInfo)->static_fields->Epsilon)
	{
		return vector;
	}

	float num2 = Vector3_Dot(vector, planeNormal, nullptr);
	
	auto returnVal = (*Vector3__TypeInfo)->static_fields->zeroVector;
	
	returnVal.x = vector.x - planeNormal.x * num2 / num;
	returnVal.y = vector.y - planeNormal.y * num2 / num;
	returnVal.z = vector.z - planeNormal.z * num2 / num;

	return returnVal;
}

Vector3 Vector3_Exclude(Vector3 excludeThis, Vector3 fromThat)
{
	return ProjectOnPlane(fromThat, excludeThis);
}
Vector3 Vector3_Create(float x, float y, float z)
{
	Vector3 vec = (*Vector3__TypeInfo)->static_fields->zeroVector;

	vec.x = x;
	vec.y = y;
	vec.z = z;

	return vec;
}

void Vector3_Set_Better(Vector3* vector, float x, float y, float z)
{
	vector->x = x;
	vector->y = y;
	vector->z = z;
}