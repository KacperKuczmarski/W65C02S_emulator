#include "src/cpu.hpp"
#include "src/memory.hpp"

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {

constexpr std::string_view usage = R"(usage: w65c02s <image> [options]

Loads a raw binary image into memory and runs it until STP or a jump-to-self
trap.

options:
  --trace           print registers before every instruction
)";

struct Options {
    std::string image;
    bool trace = false;
};

std::optional<Options> parse_args(int argc, char** argv)
{
    Options options;
    for (int i = 1; i < argc; ++i) {
        if (const std::string_view arg = argv[i]; arg == "--trace") {
            options.trace = true;
        } else if (options.image.empty() && !arg.starts_with("-")) {
            options.image = arg;
        } else {
            return std::nullopt;
        }
    }
    if (options.image.empty()) {
        return std::nullopt;
    }
    return options;
}

void print_registers(const w65c02s::Registers& r)
{
    std::printf("PC=%04X A=%02X X=%02X Y=%02X S=%02X P=%02X\n", r.pc, r.a, r.x, r.y, r.s, r.p);
}

} // namespace

int main(int argc, char** argv)
{
    const auto options = parse_args(argc, argv);
    if (!options) {
        std::fputs(usage.data(), stderr);
        return 2;
    }

    std::ifstream file(options->image, std::ios::binary);
    if (!file) {
        std::fprintf(stderr, "cannot open %s\n", options->image.c_str());
        return 2;
    }
    const std::vector<std::uint8_t> image{std::istreambuf_iterator<char>(file), {}};
    if (image.size() > w65c02s::Memory::size) {
        std::fprintf(stderr, "image is larger than 64 KiB\n");
        return 2;
    }

    w65c02s::Memory memory;
    memory.load(image);

    w65c02s::Cpu cpu(memory);
    cpu.reset();

    unsigned long long steps = 0;
    auto* reason = "";
    while (true) {
        if (options->trace) {
            print_registers(cpu.regs());
        }
        const std::uint16_t pc = cpu.regs().pc;
        cpu.step();
        ++steps;
        if (cpu.stopped()) {
            reason = "STP";
            break;
        }
        if (cpu.waiting()) {
            reason = "WAI with no interrupt source";
            break;
        }
        if (cpu.regs().pc == pc) {
            reason = "trap";
            break;
        }
    }

    std::printf("%s after %llu instructions\n", reason, steps);
    print_registers(cpu.regs());

    return 0;
}