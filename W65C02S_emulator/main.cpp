#include "Memory.h"
#include "CPU.h"

int main(void) {
	Memory mem;
	mem.init_mem();
	mem.write_test_temp();

	CPU cpu;
	cpu.Reset(mem);

	while (!cpu.no_further_inst)
	{
		cpu.Execute(mem);
	}
	
	return 0;
}