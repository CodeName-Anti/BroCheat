#include <includes.h>
#include <il2cpp-appdata.h>
#include <helpers.h>
#include <Hooks/hooks.h>
#include <Utils/magic_enum.hpp>
#include <../kiero/minhook/include/MinHook.h>
#include <Utils/gameutility.h>

using namespace app;

typedef bool (*ISPURCHASED)(String*, MethodInfo*);
typedef int (*GETPURCHASE)(String*, MethodInfo*);
typedef void (*KILLPLAYER)(PlayerSetup*, KillType__Enum, Vector3, Transform*, MethodInfo*);
typedef void (*UPDATE)(PlayerSetup*, MethodInfo*);
typedef void (*FIXEDUPDATE)(PlayerSetup*, MethodInfo*);
typedef void (*ADDMONEY)(int32_t, float, bool, MethodInfo*);
typedef void (*ADDXP)(int32_t, float, MethodInfo*);
typedef void (*ADDCROWNS)(int, float, bool, MethodInfo*);

ISPURCHASED oIsPurchased;
GETPURCHASE oGetPurchase;
KILLPLAYER oKillPlayer;
UPDATE oUpdate;
FIXEDUPDATE oFixedUpdate;
ADDMONEY oAddMoney;
ADDXP oAddXp;
ADDCROWNS oAddCrowns;

void hk_MoneyPanelScript_AddCrowns(int32_t _add, float delay, MethodInfo* method)
{
	oAddCrowns((int)((_add + 0.5 - (_add < 0)) * CheatState::rewardMultiplier), delay, true, method);
}

void hk_MoneyPanelScript_AddXp(int32_t _add, float delay, MethodInfo* method)
{
	oAddXp((int)((_add + 0.5 - (_add < 0)) * CheatState::rewardMultiplier), delay, method);
}

void hk_MoneyPanelScript_AddMoney(int32_t _add, float delay, bool setFunds, MethodInfo* method)
{
	oAddMoney((int)((_add + 0.5 - (_add < 0)) * CheatState::rewardMultiplier), delay, setFunds, method);
}

void hk_PlayerSetup_FixedUpdate(PlayerSetup* __this, MethodInfo* method)
{
	if (__this->fields.isMine)
	{
		#pragma region ===[Fly]===
		if (CheatState::fly)
		{
			auto keyboard = (*Keyboard__TypeInfo)->static_fields->_current_k__BackingField;

			auto jump = Keyboard_get_spaceKey(keyboard, nullptr);
			auto forward = Keyboard_get_wKey(keyboard, nullptr);
			auto backward = Keyboard_get_sKey(keyboard, nullptr);
			auto left = Keyboard_get_aKey(keyboard, nullptr);
			auto right = Keyboard_get_dKey(keyboard, nullptr);

			if (IsPressed(jump) ||
				IsPressed(forward) ||
				IsPressed(backward) ||
				IsPressed(left) ||
				IsPressed(right))
			{
				Rigidbody* rb = __this->fields.rb;

				Rigidbody_AddForce_1(rb, Vector3_Create(0, 65, 0), nullptr);
			}
		}
		#pragma endregion

		if (CheatState::noRagdoll)
		{
			PlayerSetup_RagdollOff(__this, nullptr);
		}
	}

	oFixedUpdate(__this, method);
}

void hk_PlayerSetup_Update(PlayerSetup* __this, MethodInfo* method)
{
	if (__this->fields.isMine)
	{
		#pragma region ===[Speed]===
		if (CheatState::speed)
		{
			__this->fields.speed = 7 * CheatState::speedValue;
			__this->fields.airVelocity = 7 * CheatState::speedValue;
		}
		else
		{
			__this->fields.speed = 7;
			__this->fields.airVelocity = 7;
		}
		#pragma endregion

		#pragma region ===[Fly]===
		if (CheatState::fly)
		{
			auto keyboard = (*Keyboard__TypeInfo)->static_fields->_current_k__BackingField;

			auto jumpKey = Keyboard_get_spaceKey(keyboard, nullptr);
			auto forwardKey = Keyboard_get_wKey(keyboard, nullptr);
			auto backwardKey = Keyboard_get_sKey(keyboard, nullptr);
			auto leftKey = Keyboard_get_aKey(keyboard, nullptr);
			auto rightKey = Keyboard_get_dKey(keyboard, nullptr);
			auto sprintKey = Keyboard_get_leftCommandKey(keyboard, nullptr);

			Rigidbody* rb = __this->fields.rb;
			Rigidbody_set_velocity(rb, (*Vector3__TypeInfo)->static_fields->zeroVector, nullptr);
			float speed = CheatState::speedValue;

			Vector3 pos = Rigidbody_get_position(rb, nullptr);

			if (IsPressed(jumpKey))
			{
				Vector3_Set_Better(&pos, pos.x, pos.y + speed, pos.z);
			}

			Transform* transform = Component_1_get_transform((Component_1*)__this, nullptr);

			Vector3 playerTransformPosVec = Transform_get_position(transform, nullptr);

			Transform* cam = Component_1_get_transform((Component_1*)Camera_get_main(nullptr), nullptr);

			Vector3 forward = Transform_get_forward(cam, nullptr);
			Vector3 up = Transform_get_up(cam, nullptr);
			Vector3 right = Transform_get_right(cam, nullptr);

			if (IsPressed(forwardKey))
			{
				Vector3_Set_Better(&pos, playerTransformPosVec.x + forward.x * up.y * speed, playerTransformPosVec.y + forward.y * speed, playerTransformPosVec.z + forward.z * up.y * speed);
			}
			if (IsPressed(backwardKey))
			{
				Vector3_Set_Better(&pos, playerTransformPosVec.x - forward.x * up.y * speed, playerTransformPosVec.y - forward.y * speed, playerTransformPosVec.z - forward.z * up.y * speed);
			}
			if (IsPressed(rightKey))
			{
				Vector3_Set_Better(&pos, playerTransformPosVec.x + right.x * speed, playerTransformPosVec.y, playerTransformPosVec.z + right.z * speed);
			}
			if (IsPressed(leftKey))
			{
				Vector3_Set_Better(&pos, playerTransformPosVec.x - right.x * speed, playerTransformPosVec.y, playerTransformPosVec.z - right.z * speed);
			}

			Rigidbody_set_position(__this->fields.rb, pos, nullptr);
		}
		#pragma endregion
		
		#pragma region ===[AirJump]===
		if (CheatState::airjump)
		{
			auto spaceKey = Keyboard_get_spaceKey((*Keyboard__TypeInfo)->static_fields->_current_k__BackingField, nullptr);
			if (WasPressedThisFrame(spaceKey))
			{
				Rigidbody* rb = __this->fields.rb;

				// Use methods because fields do not exist
				Vector3 velocity = Rigidbody_get_velocity(rb, nullptr);

				velocity.y = 10;

				Rigidbody_set_velocity(rb, velocity, nullptr);
			}
		}
		#pragma endregion

		#pragma region ===[Infinite SuperBoost]===
		if (CheatState::infiniteSuperboost)
		{
			__this->fields.hasSuperDive = true;
		}
		#pragma endregion
	}

	oUpdate(__this, method);
}

void hk_PlayerSetup_KillPlayer(PlayerSetup* __this, KillType__Enum killType, Vector3 force, Transform* killTransform, MethodInfo* method)
{
	// Skip method if godmode is enabled
	if (__this->fields.isMine && CheatState::godmode)
		return;
	oKillPlayer(__this, killType, force, killTransform, method);
}

int hk_DBManager_GetPurchase(String* productID, MethodInfo* method)
{
	if (CheatState::premium && il2cppi_to_string(productID).compare("450") == 0)
	{
		return 1000;
	}

	return oGetPurchase(productID, method);
}

bool hk_DBManager_IsPurchased(String* productID, MethodInfo* method)
{
	// return true for premium
	if (CheatState::premium && il2cppi_to_string(productID).compare("4000") == 0)
	{
		return true;
	}

	return oIsPurchased(productID, method);
}

void PrintHookError(string message)
{
	CheatLog::LogError("Error while hooking GameFunctions: " + message);
}

bool CheckAndPrintError(MH_STATUS status)
{
	if (status != MH_OK)
	{
		PrintHookError("Hooking failed with error code: " + string(magic_enum::enum_name(status)));
		return true;
	}

	return false;
}

void HookGameFunctions()
{
	MH_Initialize();

	if (CheckAndPrintError(MH_CreateHook(DBManager_IsPurchased,
		&hk_DBManager_IsPurchased, (void**)&oIsPurchased)))
	{
		return;
	}

	if (CheckAndPrintError(MH_CreateHook(PlayerSetup_KillPlayer,
		&hk_PlayerSetup_KillPlayer, (void**)&oKillPlayer)))
	{
		return;
	}

	if (CheckAndPrintError(MH_CreateHook(PlayerSetup_Update,
		&hk_PlayerSetup_Update, (void**)&oUpdate)))
	{
		return;
	}

	if (CheckAndPrintError(MH_CreateHook(PlayerSetup_FixedUpdate,
		&hk_PlayerSetup_FixedUpdate, (void**)&oFixedUpdate)))
	{
		return;
	}

	if (CheckAndPrintError(MH_CreateHook(DBManager_GetPurchase,
		&hk_DBManager_GetPurchase, (void**)&oGetPurchase)))
	{
		return;
	}

	if (CheckAndPrintError(MH_CreateHook(MoneyPanelScript_AddMoney,
		&hk_MoneyPanelScript_AddMoney, (void**)&oAddMoney)))
	{
		return;
	}

	if (CheckAndPrintError(MH_CreateHook(MoneyPanelScript_AddXp,
		&hk_MoneyPanelScript_AddXp, (void**)&oAddXp)))
	{
		return;
	}

	if (CheckAndPrintError(MH_CreateHook(MoneyPanelScript_AddCrowns,
		&hk_MoneyPanelScript_AddCrowns, (void**)&oAddCrowns)))
	{
		return;
	}

	MH_EnableHook(MH_ALL_HOOKS);
}