#include <random>
#include <array>
#include <stack>
#include <fstream>
#include <string_view>

constexpr bool DEBUG = true;


namespace chip8
{
    class Randomer 
    {
        // random seed by default
        std::mt19937 gen_;
        std::uniform_int_distribution<size_t> dist_;

    public:
        /*  ... some convenient ctors ... */
        Randomer(size_t min, size_t max, unsigned int seed = std::random_device{}())
            : gen_{seed}, dist_{min, max} {
        }

        // if you want predictable numbers
        void SetSeed(unsigned int seed) {
            gen_.seed(seed);
        }

        size_t operator()() {
            return dist_(gen_);
        }
    };


    class Core 
    {
        std::array<uint8_t,4096> memory =  // 4k Memory
        { // Loading fontset(first 80 positions)
            0xF0, 0x90, 0x90, 0x90, 0xF0, //0
            0x20, 0x60, 0x20, 0x20, 0x70, //1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
            0x90, 0x90, 0xF0, 0x10, 0x10, //4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
            0xF0, 0x10, 0x20, 0x40, 0x40, //7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
            0xF0, 0x90, 0xF0, 0x90, 0x90, //A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
            0xF0, 0x80, 0x80, 0x80, 0xF0, //C
            0xE0, 0x90, 0x90, 0x90, 0xE0, //D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
            0xF0, 0x80, 0xF0, 0x80, 0x80  //F
        }; 
        std::array<uint8_t,16>   registers;
        // Registers alias
        uint8_t reg_x;
        uint8_t reg_y;

        std::stack<uint16_t> stack;

        uint16_t pc; // program counter
        uint16_t opcode; 
        uint16_t I;

        std::array<uint8_t,64 * 32> graphics; 
        std::array<bool,16> keyboard;
        bool draw_flag;
        bool play_sound_flag;

        uint8_t delay_timer;
        uint8_t sound_timer;

        Randomer rand;

        void init(); // init or reset the emulation
        void do_operation();  

    public:
        Core() : rand(0,255) {}

        void emulate_cycle();

        bool is_key_pressed(int pos) const { return keyboard[pos]; }
        void inline set_key_pressed(int pos) { keyboard[pos] = 1; }  
        void inline release_key(int pos) { keyboard[pos] = 0; }

        bool inline play_sound() const { return play_sound_flag; }
        bool inline need_to_draw() const { return draw_flag; }
        void inline set_draw_flag() { draw_flag = false; }

        uint8_t pixel_in(int pos) const { return graphics[pos]; } 

        bool load_ROM(std::string_view filename); // TODO

    };


}
