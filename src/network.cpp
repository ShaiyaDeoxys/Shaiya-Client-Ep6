#include <hook.h>


const char* login_ip_addr = ("127.0.0.1");
auto login_ip_addr_format = "%s";

DWORD write_login_ip_addr_retn = 0x4093D4;
_declspec(naked) void write_login_ip_addr()
{
	_asm {
		push login_ip_addr
		push login_ip_addr_format
		push 0x7C0DD0
		call sprintf
		add esp, 12
		jmp write_login_ip_addr_retn
	}
}

_declspec(naked) void select_server_packet()
{
	_asm {
		mov eax, 0xA771
	}
}

_declspec(naked) void game_port()
{
	_asm {
		push 0x785A
	}
}

_declspec(naked) void login_port()
{
	_asm {
		push 0x7850
	}
}

void Network() {
	utils::Hook((LPVOID)0x4092D2, write_login_ip_addr, 5);
	//Write((LPVOID)0x5EC71F, select_server_packet, 5);
	utils::MemWrite((LPVOID)0x50C7A9, game_port, 5);
	utils::MemWrite((LPVOID)0x40AD62, login_port, 5);
	utils::MemWrite((LPVOID)0x48BE9E, login_port, 5);
	utils::MemWrite((LPVOID)0x4D4C48, login_port, 5);
}

