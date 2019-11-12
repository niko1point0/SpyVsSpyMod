#include <stdio.h>
#include <Windows.h>
#include <tlhelp32.h>

// data that is read by port #0 or port #1
// copy data from multitap to here
struct data
{
	unsigned char dpad;
	// 0xFF - 0x80 = 0x7F = left
	// 0xFF - 0x40 = 0xBF = down
	// 0xFF - 0x20 = 0xDF = right
	// 0xFF - 0x10 = 0xEF = up
	// 0xFF - 0x08 = 0xF7 = start
	// 0xFF - 0x04 = 0xFB = R3
	// 0xFF - 0x02 = 0xFD = L3
	// 0xFF - 0x01 = 0xFE = select
	// 0xFF - 0x00 = 0xFF = nothing

	unsigned char button;
	// 0xFF - 0x80 = 0x7F = square
	// 0xFF - 0x40 = 0xBF = cross
	// 0xFF - 0x20 = 0xDF = circle
	// 0xFF - 0x10 = 0xEF = triangle
	// 0xFF - 0x08 = 0xF7 = r1
	// 0xFF - 0x04 = 0xFB = l1
	// 0xFF - 0x02 = 0xFD = r2
	// 0xFF - 0x01 = 0xFE = l2
	// 0xFF - 0x00 = 0xFF = nothing

	// 0xFF for max
	// 0x7F for middle
	// 0x00 for min
	unsigned char rightX;
	unsigned char rightY;
	unsigned char leftX;
	unsigned char leftY;

	// set to 0xFF to activate
	// set to 0x00 to deactivate
	unsigned char dPadRight;
	unsigned char dPadLeft;
	unsigned char dPadUp;
	unsigned char dPadDown;
	unsigned char triangle;
	unsigned char circle;
	unsigned char cross;
	unsigned char square;
	unsigned char l1;
	unsigned char r1;
	unsigned char l2;
	unsigned char r2;
};

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

	printf("Step 1: Choose Multiplayer\n");
	printf("Step 2: Choose Local\n");
	printf("Step 3: Start a 1-player match with 0 AI\n");
	printf("Step 4: Enjoy the 4-player game\n");

	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);

	data test1;
	data empty;

	empty.dpad = 0xFF;
	empty.button = 0xFF;
	empty.rightX = 0x7F;
	empty.rightY = 0x7F;
	empty.leftX = 0x7F;
	empty.leftY = 0x7F;
	empty.dPadRight = 0x00;
	empty.dPadLeft = 0x00;
	empty.dPadUp = 0x00;
	empty.dPadDown = 0x00;
	empty.triangle = 0x00;
	empty.circle = 0x00;
	empty.cross = 0x00;
	empty.square = 0x00;
	empty.l1 = 0x00;
	empty.r1 = 0x00;
	empty.l2 = 0x00;
	empty.r2 = 0x00;

	// set index data
	const int p1 = 0;
	const int p2 = 1;
	const int p3 = 2;
	const int p4 = 3;
	const int junk = 4;

	while (true)
	{
		// Get the state of the game
		// -1 = loading
		//  0 = main "Press Start" screen
		//  1 = selection of single player, multiplayer, options, extras
		//  2 = selection of local, online
		//  4 = 4-player menu where everyone presses start
		//  3 = gameplay
		
		// not sure why it is like that, but it is

		// Read the state from RAM
		int menuState = 0;
		ReadProcessMemory(handle, (PBYTE*)(0x20434050), &menuState, sizeof(int), 0); // p4

		// If the game is loading, set everything to 4-players
		if (menuState == -1)
		{
			// player ID in menus
			// This makes the computer think all players are connected
			WriteProcessMemory(handle, (PBYTE*)(0x203D8910), &p1, sizeof(int), 0); // p1
			WriteProcessMemory(handle, (PBYTE*)(0x203D8914), &p1, sizeof(int), 0); // p2
			WriteProcessMemory(handle, (PBYTE*)(0x203D8918), &p1, sizeof(int), 0); // p3
			WriteProcessMemory(handle, (PBYTE*)(0x203D891C), &p1, sizeof(int), 0); // p4

			// Number of players in the game, and number of players in menu
			float x = 4;
			//WriteProcessMemory(handle, (PBYTE*)(0x207EB35C), &x, sizeof(float), 0);
			WriteProcessMemory(handle, (PBYTE*)(0x207D0B78), &x, sizeof(float), 0);

			// This is required because... why?
			// It wont work without it
			Sleep(50);
		}

		//==========================================================================================
		// Write to controller 2, while it is plugged
		// Write will not break it
		//
		// Write to controller 2, while it is unplugged.
		// Without Write = the game thinks 4 controllers are there
		// Calling Write = the game knows controllers aren't there
		//
		// Both controllers must be plugged, can have 0 bindings
		// Then controller support can be outside of PCSX2
		// When the game sees either of 2 controllers unplugged
		// it realizes that players 3 and 4 are unplugged too
		//==========================================================================================

		if (menuState == 3)
		{
			// Buffer 1 -> Player 1
			// Buffer 2 -> Player 2
			// Buffer 3 -> Player 3		where is Buffer 3?
			// Buffer 4 -> Player 4		where is Buffer 4?
			WriteProcessMemory(handle, (PBYTE*)(0x20433860), &p1, sizeof(int), 0); // p1
			WriteProcessMemory(handle, (PBYTE*)(0x204338A8), &p1, sizeof(int), 0); // p2
			WriteProcessMemory(handle, (PBYTE*)(0x204338F0), &p1, sizeof(int), 0); // p3
			WriteProcessMemory(handle, (PBYTE*)(0x20433938), &p1, sizeof(int), 0); // p4
		}

		// Create a controller buffer
		// This will be copied from player 1 to other players as test
		char fullBuffer[0x388];
		memset(fullBuffer, 0, 0x388);

		// In the future, buffers will be generated from controller input
		// that is extracted from the PCSX2 multitap driver. Then the game
		// will ignore the multitap, and this software will redirect the input
		// into the proper buffers, to make the game playable.

		// Options 1 + 2 together work flawlessly when P2 is plugged in,
		// make it work without plugged controllers, and disable the 
		// checks for the controller plugged in

		// This works by copying buffers in the game's ram, not
		// by copying buffers in the controller plugin's ram, making
		// it potentially possible to make all 4 players usable

		// Option 1
		// copy buffers from player 1 to player 2
		// If attempted on an unplugged controller, there is no effect
		// If attempted on a plugged controller, it works almost flawless, 
		//		but triggers detection for all unplugged players
		ReadProcessMemory(handle, (PBYTE*)(0x20440842 + 0x140 * 0), fullBuffer, 0x140, 0);
		WriteProcessMemory(handle, (PBYTE*)(0x20440842 + 0x140 * 1), fullBuffer, 0x140, 0);
		//WriteProcessMemory(handle, (PBYTE*)(0x20440842 + 0x140 * 2), fullBuffer, 0x140, 0);
		//WriteProcessMemory(handle, (PBYTE*)(0x20440842 + 0x140 * 3), fullBuffer, 0x140, 0);

		// Option 2
		// Works with player 2 when controller is plugged
		// Copying to any player will trigger a check to see if 
		// controller is plugged, and will crash when unplugged
		ReadProcessMemory(handle, (PBYTE*)(0x20448110 + 0x388 * 0), fullBuffer, 0x388, 0);
		WriteProcessMemory(handle, (PBYTE*)(0x20448110 + 0x388 * 1), fullBuffer, 0x388, 0);
		//WriteProcessMemory(handle, (PBYTE*)(0x20448110 + 0x388 * 2), fullBuffer, 0x388, 0);
		//WriteProcessMemory(handle, (PBYTE*)(0x20448110 + 0x388 * 3), fullBuffer, 0x388, 0);

		// Option 3
		// Writing here for 3 and 4 makes no input functional for anybody
		//ReadProcessMemory(handle, (PBYTE*)(0x24055E71 + 0x710 * 0), fullBuffer, 0x170, 0);
		//WriteProcessMemory(handle, (PBYTE*)(0x24055E71 + 0x710 * 1), fullBuffer, 0x170, 0);
		
		// Option 4
		//==========================================================================================
		// Swap controller per frame
		// Two players must be plugged
		// Pressing 5-8 makes them jump
		//==========================================================================================
		
		/*
		// If you're in gameplay
		if (menuState == 3)
		{
			// Buffer 1 -> Player 1
			// Buffer 2 -> Player 2
			WriteProcessMemory(handle, (PBYTE*)(0x20433860), &p1, sizeof(int), 0); // p1
			WriteProcessMemory(handle, (PBYTE*)(0x204338A8), &p2, sizeof(int), 0); // p2
			WriteProcessMemory(handle, (PBYTE*)(0x204338F0), &junk, sizeof(int), 0); // p3
			WriteProcessMemory(handle, (PBYTE*)(0x20433938), &junk, sizeof(int), 0); // p4
		}
		
		// Controller 1
		// read multitap 1-A
		// use that to generate "data" struct
		memcpy(&test1, &empty, sizeof(data));
		if (GetAsyncKeyState(53)) // if you press 5
			test1.cross = 0xFF;

		// Controller 1 -> Buffer 1 -> Player 1
		// write to 0x20440842 + 0x140 * 0
		WriteProcessMemory(handle, (PBYTE*)(0x20440842 + 0x140 * 0), &test1, sizeof(data), 0);

		// Controller 2
		// read multitap 1-B
		// use that to generate "data" struct
		memcpy(&test1, &empty, sizeof(data));
		if (GetAsyncKeyState(54)) // if you press 6
			test1.cross = 0xFF;

		// Controller 2 -> Buffer 2 -> Player 2
		// write to 0x20440842 + 0x140 * 1
		WriteProcessMemory(handle, (PBYTE*)(0x20440842 + 0x140 * 1), &test1, sizeof(data), 0);
		
		// sleep 1 frame, (1000 / 50) ms
		Sleep(20);

		// If you're in gameplay
		if (menuState == 3)
		{
			// Buffer 1 -> Player 3
			// Buffer 2 -> Player 4
			WriteProcessMemory(handle, (PBYTE*)(0x20433860), &junk, sizeof(int), 0); // p1
			WriteProcessMemory(handle, (PBYTE*)(0x204338A8), &junk, sizeof(int), 0); // p2
			WriteProcessMemory(handle, (PBYTE*)(0x204338F0), &p1, sizeof(int), 0); // p3
			WriteProcessMemory(handle, (PBYTE*)(0x20433938), &p2, sizeof(int), 0); // p4
		}

		// Controller 3
		// read multitap 1-C
		// use that to generate "data" struct
		memcpy(&test1, &empty, sizeof(data));
		if (GetAsyncKeyState(55)) // if you press 7
			test1.cross = 0xFF;

		// Controller 3 -> Buffer 1 -> Player 3
		// write to 0x20440842 + 0x140 * 0
		WriteProcessMemory(handle, (PBYTE*)(0x20440842 + 0x140 * 0), &test1, sizeof(data), 0);

		// Controller 4
		// read multitap 1-D
		// use that to generate "data" struct
		memcpy(&test1, &empty, sizeof(data));
		if (GetAsyncKeyState(56)) // if you press 8
			test1.cross = 0xFF;

		// Controller 4 -> Buffer 2 -> Player 4
		// write to 0x20440842 + 0x140 * 1
		WriteProcessMemory(handle, (PBYTE*)(0x20440842 + 0x140 * 1), &test1, sizeof(data), 0);

		// sleep 1 frame, (1000 / 50) ms
		Sleep(20);*/
	}
}
