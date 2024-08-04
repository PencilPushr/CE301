// i80386.cpp : Defines the entry point for the application.
#include "i80386.h"

using namespace std;
using i386 = i80386;

int main()
{
	i386* emulator = new i386;
    // TODO: add request function
	emulator->Run();
	delete emulator;

	return 0;
}
