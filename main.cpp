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

Loads a raw binary image into memory and runs it until STP, a jump-to-self
trap, or the step limit.

options:
  --load ADDR       load address of the image (default 0x0000)
  --pc ADDR         start address (default: reset vector at $FFFC)
  --max-steps N     stop after N instructions (default 100000000)
  --expect-pc ADDR  exit with 0 only if execution ends at ADDR
  --trace           print registers before every instruction
)";

struct Options {
    std::string image;
    std::uint16_t load_address = 0x0000;
    std::optional<std::uint16_t> start;
    std::optional<std::uint16_t> expect_pc;
    unsigned long long max_steps = 100'000'000;
    bool trace = false;
};

std::optional<unsigned long long> parse_number(const char* text)
{
    try {
        std::size_t used = 0;
        const unsigned long long value = std::stoull(text, &used, 0);
        if (text[used] != '\0') {
            return std::nullopt;
        }
        return value;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::optional<std::uint16_t> parse_address(const char* text)
{
    const auto value = parse_number(text);
    if (!value || *value > 0xFFFF) {
        return std::nullopt;
    }
    return static_cast<std::uint16_t>(*value);
}

std::optional<Options> parse_args(int argc, char** argv)
{
    Options options;
    for (int i = 1; i < argc; ++i) {
        const std::string_view arg = argv[i];
        const bool has_value = i + 1 < argc;

        if (arg == "--trace") {
            options.trace = true;
        } else if (arg == "--load" && has_value) {
            const auto address = parse_address(argv[++i]);
            if (!address) {
                return std::nullopt;
            }
            options.load_address = *address;
        } else if (arg == "--pc" && has_value) {
            options.start = parse_address(argv[++i]);
            if (!options.start) {
                return std::nullopt;
            }
        } else if (arg == "--expect-pc" && has_value) {
            options.expect_pc = parse_address(argv[++i]);
            if (!options.expect_pc) {
                return std::nullopt;
            }
        } else if (arg == "--max-steps" && has_value) {
            const auto steps = parse_number(argv[++i]);
            if (!steps) {
                return std::nullopt;
            }
            options.max_steps = *steps;
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
    if (options->start) {
        cpu.regs().pc = *options->start;
    }

    unsigned long long steps = 0;
    const char* reason = "step limit reached";
    while (steps < options->max_steps) {
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

    if (options->expect_pc) {
        return cpu.regs().pc == *options->expect_pc ? 0 : 1;
    }
    return 0;
}