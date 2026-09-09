#include "Memory.h"
#include "CPU.h"


int main() {
	Memory mem{};
	mem.init_mem();
	mem.write_test_temp();
	//mem.write_test_program_pow2();

	CPU cpu;
	cpu.reset(mem);

	while (!cpu.no_further_inst)
	{
		cpu.execute(mem);
	}
	return 0;
}