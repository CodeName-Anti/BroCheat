#pragma once
#include <pch-il2cpp.h>

void DisableAntiCheat();

app::PlayerSetup* GetLocalPlayer();

bool WasPressedThisFrame(app::KeyControl* key);
bool IsPressed(app::KeyControl* key);

std::string FormatPlayername(app::PlayerSetup* player);

// Vector3
app::Vector3 Vector3_Exclude(app::Vector3 excludeThis, app::Vector3 fromThat);
app::Vector3 Vector3_Create(float x, float y, float z);
void Vector3_Set_Better(app::Vector3* vector, float x, float y, float z);