#include <hook.h>

char szLocationX[255];
char szLocationY[255];
char newLocationX[255];
char newLocationY[255];

int LocationX;
int LocationY;
int oldLocationX;
int oldLocationY;

int MouseX;
int MouseY;

DWORD mouseReturn = 0x4D89F8;
DWORD newlocationReturn = 0x4D8B09;

void LocationConfig() {
	GetPrivateProfileStringA("BUFF", "LOCATION_X", "", szLocationX, 255, ".\\CONFIG.ini");
	GetPrivateProfileStringA("BUFF", "LOCATION_Y", "", szLocationY, 255, ".\\CONFIG.ini");
	LocationX = atoi(szLocationX);
	LocationY = atoi(szLocationY);
	oldLocationX = atoi(szLocationX);
	oldLocationY = atoi(szLocationY);
}

void SaveLocation() {
	_itoa_s(LocationX, newLocationX, 10);
	_itoa_s(LocationY, newLocationY, 10);

	if (oldLocationX != LocationX) {
		WritePrivateProfileStringA("BUFF", "LOCATION_X", newLocationX, ".\\CONFIG.ini");
		WritePrivateProfileStringA("BUFF", "LOCATION_Y", newLocationY, ".\\CONFIG.ini");
	}
}



__declspec(naked) void Mouse()
{
	_asm
	{
		push ebx
		mov ebx, 0x7C3C0C
		mov ebx, [ebx]
		mov[MouseX], ebx
		mov ebx, 0x7C3C10
		mov ebx, [ebx]
		mov[MouseY], ebx
		pop ebx
		mov edi, [LocationX]
		mov ebx, [LocationY]
		jmp mouseReturn

	}
}

__declspec(naked) void GetLocation()
{
	_asm
	{
		push esi
		mov esi, GetAsyncKeyState
		push edi
		mov edi, 0x008000
		lea eax, [ecx + 0x00]
		locationReturn2:
		push 0x10
			call esi
			test di, ax
			je locationReturn2
			push 0x02
			call esi
			test di, ax
			je locationReturn2
			mov eax, [MouseX]
			mov[LocationX], eax
			mov eax, [MouseY]
			mov[LocationY], eax
			jmp locationReturn2
	}
}

__declspec(naked) void NewLocation()
{
	_asm
	{
		mov edi, [LocationX]
		jmp newlocationReturn
	}
}

void UpdateLocations() {
	while (true) {
		SaveLocation();
		Sleep(1000);
	}
}

void Buff_Icons() {
	SaveLocation();
	LocationConfig();
	utils::Hook((LPVOID)0x4D73ED, Mouse, 11);
	utils::Hook((LPVOID)0x4D7503, NewLocation, 6);
	CreateThread(NULL, NULL, LPTHREAD_START_ROUTINE(GetLocation), NULL, 0, 0);
	CreateThread(NULL, NULL, LPTHREAD_START_ROUTINE(UpdateLocations), NULL, 0, 0);

}








