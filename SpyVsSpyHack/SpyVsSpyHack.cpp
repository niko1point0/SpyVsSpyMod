#include <stdio.h>
#include <Windows.h>
#include <tlhelp32.h>

DWORD GetProcId(char* ProcName)
{
	PROCESSENTRY32   pe32;
	HANDLE         hSnapshot = NULL;

	pe32.dwSize = sizeof(PROCESSENTRY32);
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (Process32First(hSnapshot, &pe32))
	{
		do
		{
			char* test[18];
			for (int i = 0; i < 18; i++)
				test[i] = (char*)&pe32.szExeFile + i;

			if (*test[0] == 'p' &&
				*test[2] == 'c' &&
				*test[4] == 's' &&
				*test[6] == 'x' &&
				*test[8] == '2' &&
				*test[10] == '.' &&
				*test[12] == 'e' &&
				*test[14] == 'x' &&
				*test[16] == 'e')

				return pe32.th32ProcessID;

		} while (Process32Next(hSnapshot, &pe32));
	}

	if (hSnapshot != INVALID_HANDLE_VALUE)
		CloseHandle(hSnapshot);
	return 0;
}

int main()
{
	DWORD procID = GetProcId((char*)"pcsx2.exe");

	if (!procID)
	{
		printf("Failed to find pcsx2.exe\n");
		printf("open pcsx2.exe, launch Spy Vs Spy\n");
		printf("and try again\n");

		system("pause");
		exit(0);
	}

	printf("This does not work yet, read comments\n");

	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);

	while (true)
	{
		// Number of players in the game, and number of players in menu
		float x = 4;
		WriteProcessMemory(handle, (PBYTE*)(0x207EB35C), &x, sizeof(float), 0);
		WriteProcessMemory(handle, (PBYTE*)(0x207D0B78), &x, sizeof(float), 0);


		// Create a controller buffer
		// This will be copied from player 1 to other players as test
		char fullBuffer[0x388];
		memset(fullBuffer, 0, 0x388);

		// FUN_001e5af0 only uses two of these, which is why P2 works fine,
		// even when emulator views P2 as "unplugged" instead of "dualshock 2"
		ReadProcessMemory (handle, (PBYTE*)(0x20440800 + 0x140 * 0), fullBuffer, 0x140, 0);
		WriteProcessMemory(handle, (PBYTE*)(0x20440800 + 0x140 * 1), fullBuffer, 0x140, 0);
		WriteProcessMemory(handle, (PBYTE*)(0x20440800 + 0x140 * 2), fullBuffer, 0x140, 0);
		WriteProcessMemory(handle, (PBYTE*)(0x20440800 + 0x140 * 3), fullBuffer, 0x140, 0);

		/*
			Fix
			
			Change "} while (iVar2 < 2);" to "} while (iVar2 < 4);"
			Change "} while ((int)puVar3 < 0x440a80);" to "} while ((int)puVar3 < 0x440D00);"
		
		*/

		// FUN_0021d010 uses all four of these, which is why "Start" button works for everyone,
		// but then P3 and P4 get disconnected cause of the problem with the first buffer system.
		ReadProcessMemory (handle, (PBYTE*)(0x20448132 + 0x388 * 0), fullBuffer, 0x388, 0);
		WriteProcessMemory(handle, (PBYTE*)(0x20448132 + 0x388 * 1), fullBuffer, 0x388, 0);
		WriteProcessMemory(handle, (PBYTE*)(0x20448132 + 0x388 * 2), fullBuffer, 0x388, 0);
		WriteProcessMemory(handle, (PBYTE*)(0x20448132 + 0x388 * 3), fullBuffer, 0x388, 0);
	}
}
