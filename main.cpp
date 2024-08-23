#include <windows.h>
#include "toml++/toml.hpp"
#include "nya_commonhooklib.h"

int nZoomKey = VK_F1;
int nHUDKey = VK_F2;

int nMapZoomState = 4;
uint32_t bHUDToggle = 1;
float fHUDToggleScale = 1.0;

struct tMapZoomSetup {
	float values[12];
};
tMapZoomSetup aMapZooms[] = {
		{6.0, -205.0, 128.0, 128.0, 15.0, 170.0, -40.0, -290.0, 256.0, 256.0, 15.0, 250.0},
		{-15.0, -285.0, 192.0, 192.0, 15.0, 250.0, -15.0, -285.0, 256.0, 256.0, 15.0, 200.0},
		{-15.0, -285.0, 256.0, 256.0, 15.0, 330.0, -15.0, -285.0, 256.0, 256.0, 15.0, 150.0},
		{-15.0, -285.0, 192.0, 192.0, 8.0, 637.0, -15.0, -285.0, 256.0, 256.0, 8.0, 637.0},
		{-15.0, -285.0, 256.0, 256.0, 8.0, 637.0, -15.0, -285.0, 256.0, 256.0, 8.0, 637.0}
};
const int nNumMapZooms = sizeof(aMapZooms) / sizeof(aMapZooms[0]);

uintptr_t HUDToggleASM1_jmp = 0x477EC9;
void __attribute__((naked)) HUDToggleASM1() {
	__asm__ (
		"mov eax, [esi+0x2DC]\n\t"
		"cmp eax, 0\n\t"
		"jz loc_jmpout\n\t"
		"mov eax, %1\n\t"
		"loc_jmpout:"
		"jmp %0\n\t"
			:
			:  "m" (HUDToggleASM1_jmp), "m" (bHUDToggle)
	);
}

uintptr_t HUDToggleASM2_jmp = 0x4982C9;
void __attribute__((naked)) HUDToggleASM2() {
	__asm__ (
		"call dword ptr [eax+0x80]\n\t"
		"mov eax, %1\n\t"
		"mov [ebp+0x18], eax\n\t"
		"jmp %0\n\t"
			:
			:  "m" (HUDToggleASM2_jmp), "m" (fHUDToggleScale)
	);
}

void __fastcall KeyboardHook(uint16_t keyCode) {
	(*(uint32_t*)0x8DA740)++;
	if (((*(uint32_t*)0x8E84B0) & 1) == 0) return; // in race check

	if (keyCode == nZoomKey) {
		nMapZoomState--;
		if (nMapZoomState < 0) nMapZoomState = nNumMapZooms - 1;

		memcpy((void*)0x8DC664, &aMapZooms[nMapZoomState], sizeof(tMapZoomSetup));
	}
	else if (keyCode == nHUDKey) {
		fHUDToggleScale = (bHUDToggle = !bHUDToggle) ? 1.0 : 0.0;
	}
}

uintptr_t KeyboardHookSetupASM_jmp = 0x55AB50;
void __attribute__((naked)) KeyboardHookSetupASM() {
	__asm__ (
		"pushad\n\t"
		"call %1\n\t"
		"popad\n\t"

		"jmp %0\n\t"
			:
			:  "m" (KeyboardHookSetupASM_jmp), "i" (KeyboardHook)
	);
}

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
	switch( fdwReason ) {
		case DLL_PROCESS_ATTACH: {
			if (NyaHookLib::GetEntryPoint() != 0x202638) {
				MessageBoxA(nullptr, "Unsupported game version! Make sure you're using DRM-free v1.2 (.exe size of 2990080 bytes)", "nya?!~", MB_ICONERROR);
				exit(0);
				return TRUE;
			}

			auto config = toml::parse_file("FlatOut2MapZoom_gcp.toml");
			nZoomKey = config["main"]["map_zoom_key"].value_or(VK_F1);
			nHUDKey = config["main"]["hud_toggle_key"].value_or(VK_F2);

			NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x477EC3, &HUDToggleASM1);
			NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4982C3, &HUDToggleASM2);
			NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x55AB4A, &KeyboardHookSetupASM);
		} break;
		default:
			break;
	}
	return TRUE;
}