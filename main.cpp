#include <windows.h>
#include "nya_commonhooklib.h"

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

int nMapZoomValue;
int __fastcall MapZoomSetup(uint32_t _zoomValue) {
	auto zoomValue = *(float*)&_zoomValue;
	if (zoomValue > 0.8f) return nMapZoomValue = -65536;
	if (zoomValue > 0.6f) return nMapZoomValue = -84644;
	if (zoomValue > 0.4f) return nMapZoomValue = -13252;
	if (zoomValue > 0.2f) return nMapZoomValue = -7819;
	if (zoomValue > 0.1f) return nMapZoomValue = -64;
	return nMapZoomValue = -1;
}

uintptr_t MapZoomSetupASM_jmp = 0x4BDCFB;
void __attribute__((naked)) MapZoomSetupASM() {
	__asm__ (
		"cmp ebx, [esp+0xCC]\n\t"
		"mov eax, [esp+0x30]\n\t"
		"jz loc_4BDCFB\n\t"

		"pushad\n\t"
		"mov ecx, [ebx+0x33C]\n\t"
		"mov ecx, [ecx+0x6AA0]\n\t"
		"call %1\n\t"
		"popad\n\t"
		"mov eax, %2\n\t"

		"loc_4BDCFB:\n\t"
		"jmp %0\n\t"
			:
			:  "m" (MapZoomSetupASM_jmp), "i" (MapZoomSetup), "m" (nMapZoomValue)
	);
}

void __fastcall KeyboardHook(uint16_t keyCode) {
	(*(uint32_t*)0x8DA740)++;
	if (((*(uint32_t*)0x8E84B0) & 1) == 0) return; // in race check

	// map zoom
	if (keyCode == VK_F1) {
		nMapZoomState--;
		if (nMapZoomState < 0) nMapZoomState = nNumMapZooms - 1;

		memcpy((void*)0x8DC664, &aMapZooms[nMapZoomState], sizeof(tMapZoomSetup));
	}
	// HUD toggle
	else if (keyCode == VK_F2) {
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
				MessageBoxA(nullptr, "Unsupported game version! Make sure you're using v1.2 (.exe size of 2990080 bytes)", "nya?!~", MB_ICONERROR);
				return TRUE;
			}

			NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x477EC3, &HUDToggleASM1);
			NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4982C3, &HUDToggleASM2);
			NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4BDCEA, &MapZoomSetupASM);
			NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x55AB4A, &KeyboardHookSetupASM);
		} break;
		default:
			break;
	}
	return TRUE;
}