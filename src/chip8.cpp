#include "chip8.hpp"


void chip8::Core::init()
{
    pc = 0x200;
    opcode = 0;
    delay_timer = 0;
    sound_timer = 0;
    I = 0;

    init_opcodes();

    for(int i = 0; i < 16; i++)
    {
        keyboard[i] = false;
        registers[i] = false;

        if( !stack.empty() ) 
            stack.pop();

    }


    // Cleaning memory
    for(int i = 80; i < memory.size(); i++) 
        memory[i] = 0;

    

}

void chip8::Core::do_operation()
{

    // Based in operations
    //https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Instruction-Set
    switch(opcode & 0xF000) // 0x0NNN
    {
        case 0x0000:

            switch(opcode & 0x000F)
            {
                // 0x00E0
                case 0x0000:
                    for(int i = 0; i < 2048; i++)
                        graphics[i] = 0;

                    draw_flag = true;
                    pc += 2;
                    break;

                case 0x000E: // Return from a subroutine
                    pc = stack.top();
                    stack.pop();
                    pc += 2;
                    break;

                default: // Unknown opcode
                    std::cerr << "Unknown opcode: " << std::hex << opcode << '\n';
                    exit(3);
            }
            break;

        case 0x1000: // Jump to address 0x0NNN
            pc = opcode & 0x0FFF;
            break;

        case 0x2000:
            stack.push(pc); // saving program counter(start point)
            pc = opcode & 0x0FFF; // Jump to address 0x0NNN
            break;

        // skip following instruction if VX == NN
        // 0x3XNN
        case 0x3000:
            reg_x = (opcode & 0x0F00) >> 8;

            if( registers[reg_x] == (opcode & 0x00FF) ) 
                pc += 4; // skip instruction
            else
                pc += 2; // go to next instruction
            break;

        // skip following instruction if VX != NN
        // 0x4XNN
        case 0x4000:
            reg_x = (opcode & 0x0F00) >> 8;

            if( registers[reg_x] != (opcode & 0x00FF) ) 
                pc += 4; // skip instruction
            else
                pc += 2; // go to next instruction
            break;

        // skip following instruction if VX == VY
        // 0x5XY0
        case 0x5000:
            reg_x = (opcode & 0x0F00) >> 8;
            reg_y = (opcode & 0x00F0) >> 4;

            if( registers[reg_x] == registers[reg_y] )
                pc += 4; // skip instruction
            else
                pc += 2; // go to next instruction
            break;

        // Store number NN in VX
        // 0x6XNN
        case 0x6000:
            reg_x = (opcode & 0x0F00) >> 8;
            registers[reg_x] = (opcode & 0x00FF);
            pc += 2; // next instruction
            break; 

        // add number NN to VX
        // 0x7XNN
        case 0x7000:
            reg_x = (opcode & 0x0F00) >> 8;
            registers[reg_x] += (opcode & 0x00FF);
            pc += 2; // next instruction
            break; 

        case 0x8000:
            reg_x = (opcode & 0x0F00) >> 8;
            reg_y = (opcode & 0x00F0) >> 4;

            switch(opcode & 0x000F)
            {
                // Store the value of register VY in register VX
                // 0x8XY0
                case 0x0000:
                    registers[reg_x] = registers[reg_y];
                    pc += 2;
                    break;

                // Set VX to VX OR VY
                // 0x8XY1
                case 0x0001:
                    registers[reg_x] |= registers[reg_y];
                    pc += 2;
                    break;

                // Set VX to VX AND VY
                // 0x8XY2
                case 0x0002:
                    registers[reg_x] &= registers[reg_y];
                    pc += 2;
                    break;
                    
                // Set VX to VX XOR VY
                // 0x8XY3
                case 0x0003:
                    registers[reg_x] ^= registers[reg_y];
                    pc += 2;
                    break;


                // 0x8XY4
                case 0x0004:
                    if( registers[reg_y] > (0xFF - registers[reg_x]))
                        registers[0xF] = 1;
                    else 
                        registers[0xF] = 0;

                    registers[reg_x] += registers[reg_y];

                    pc += 2;
                    break;

                // 0x8XY5
                case 0x0005:
                    if( registers[reg_y] > registers[reg_x])
                        registers[0xF] = 0; // borrow
                    else 
                        registers[0xF] = 1;

                    registers[reg_x] -= registers[reg_y];
                    pc += 2;
                    break;

                // 0x8XY6
                case 0x0006:
                    registers[0xF] = registers[reg_x] & 0x1; // VF gets VX least significant bit
                    registers[reg_x] >>= 1; // shift VX one bit to right 
                    pc += 2;
                    break;

                // 0x8XY7
                case 0x0007:
                    if( registers[reg_y] > registers[reg_x])
                        registers[0xF] = 0; // borrow
                    else 
                        registers[0xF] = 1;

                    registers[reg_x] -= registers[reg_y];
                    pc += 2;
                    break;

                // 0x8XYE
                case 0x000E:
                    registers[0xF] = registers[reg_x] >> 7; // VF gets VX most significant bit
                    registers[reg_x] <<= 1; 
                    pc += 2;
                    break;
                
                default: // Unknow opcode
                    std::cerr << "Unknown opcode: " << std::hex << opcode << '\n';
                    exit(3);
                    break;


            }
            break;


        // Skip following instruction if VX != VY
        case 0x9000:
            reg_x = (opcode & 0x0f00) >> 8;
            reg_y = (opcode & 0x00f0) >> 4;

            if( registers[reg_x] != registers[reg_y] )
                pc += 4; // skip instruction
            else
                pc += 2; // go to next instruction
            break;

        //Store memory address NNN in register I
        // 0xANNN
        case 0xA000:
            I = opcode & 0x0FFF;
            pc += 2;
            break;

        // Jump to address NNN + V0
        // 0xBNNN
        case 0xB000:
            pc = (opcode & 0x0FFF) + registers[0];
            break;

        // Set VX to a random number with a mask of NN 
        // 0xCXNN
        case 0xC000:
            reg_x = (opcode & 0x0f00) >> 8;
            registers[reg_x] = rand() & 0x00FF;
            pc += 2;
            break;


        // 0xDXYN
        case 0xD000:
        {
            reg_x = (opcode & 0x0f00) >> 8;
            reg_y = (opcode & 0x00f0) >> 4;

            uint8_t x = registers[reg_x];
            uint8_t y = registers[reg_y];
            uint8_t height = opcode & 0x000F;
            uint8_t pixel;

            registers[0xF] = 0;
            for(int yline = 0; yline < height; yline++)
            {
                pixel = memory[I + yline];
                for(int xline = 0; xline < 8; xline++)
                {
                    if((pixel & (0x80 >> xline)) != 0)
                    {
                        if(graphics[(x+ xline + ((y+yline) * 64))] == 1)
                            registers[0xF] = 1;
                        graphics[(x+ xline + ((y+yline) * 64))] ^= 1;
                    }
                }
            }

            draw_flag = true;
            pc += 2;
            break;
        }
        
        // 0xEX**
        case 0xE000:
            reg_x = (opcode & 0x0f00) >> 8;
            switch( opcode & 0x00FF )
            {
                // 0xEX9E
                case 0x009E:
                    if( keyboard[ registers[reg_x] ] == true)  // key is pressed
                        pc += 4;
                    else 
                        pc += 2;
                    break;

                // 0xEXA1
                case 0x00A1:
                    if( keyboard[ registers[reg_x] ] == false)  // key not pressed
                        pc += 4;
                    else 
                        pc += 2;
                    break;

                default: // Unknonw opcode
                    std::cerr << "Unknown opcode: " << std::hex << opcode << '\n';
                    exit(3);
            }
            break;

       
        //0xFX**
        case 0xF000:
            reg_x = (opcode & 0x0F00) >> 8;
            switch( opcode & 0x00FF )
            {
                case 0x0007:
                    registers[ reg_x ] = delay_timer;
                    pc += 2;
                    break;
                case 0x000A:
                {
                    bool key_pressed = false;

                    for(int i = 0; i < keyboard.size(); i++)
                    {
                        if(keyboard[i] != 0)
                        {
                            registers[reg_x] = i;
                            key_pressed = true;
                        }
                    }

                    if(!key_pressed)
                        return;

                    pc += 2;
                }
                    break; 

                case 0x0015:
                    delay_timer = registers[ reg_x ];
                    pc += 2;
                    break;

                case 0x0018:
                    sound_timer = registers[ reg_x ];
                    pc += 2;
                    break;

                case 0x001E:
                    if( I + registers[ reg_x ] > 0xFFF)
                        registers[0xF] = 1;
                    else 
                        registers[0xF] = 0;
                    I += registers[ reg_x ];

                    pc += 2;
                    break;

                case 0x0029:
                    I =  registers[reg_x] * 0x5;
                    pc += 2;
                    break;

                case 0x0033:
                    memory[I] = registers[ reg_x ] / 100;
                    memory[I + 1] = (registers[ reg_x ] / 10) % 10;
                    memory[I + 2] = registers[ reg_x ]  % 10;
                    pc += 2;
                    break;

                case 0x0055:
                    for( int i = 0; i <= reg_x; i++)
                        memory[I + i] = registers[i];

                    I += reg_x + 1;
                    pc += 2;
                    break;

                case 0x0065:
                    for( int i = 0; i <= reg_x; i++)
                        registers[i] =  memory[I + i];


                    I += reg_x + 1;
                    pc += 2;
                    break;

                default://Unkwnonw opcode
                    std::cerr << "Unknown opcode: " << std::hex << opcode << '\n';
                    exit(3);

            }
            break;

        default://Unkwnonw opcode
            std::cerr << "Unknown opcode: " << std::hex << opcode << '\n';
            exit(3);
            break;

    }
}

void chip8::Core::emulate_cycle()
{
    // fetch opcode 
    opcode = memory[pc] << 8 | memory[pc+1];
    
    reg_x = (opcode & 0x0f00) >> 8;
    reg_y = (opcode & 0x00f0) >> 4;
    if( operations[ opcode & 0xF000 ] )
    {
        std::cout << "Doing operation: " << std::hex << opcode << '\n';
        operations[opcode & 0xF000]();
    }
    else
        std::cerr << "Unknown opcode: " << std::hex << opcode << '\n';

    // Update timers
    if (delay_timer > 0)
        --delay_timer;

    if (sound_timer > 0)
    {
        if(sound_timer == 1);
            sound_flag = true;

        --sound_timer;
    }
    else
        sound_flag = false;

}

void chip8::Core::init_opcodes()
{
    operations[0x0000] = [&]()
    {
        operations[opcode & 0x00FF]();
    };
    operations[0x00E0] = [&]()
    {
        for(int i = 0; i < 2048; i++)
            graphics[i] = 0;

        draw_flag = true;
        pc += 2;
    };
    operations[0x00EE] = [&]()
    {
        pc = stack.top();
        stack.pop();
        pc += 2;
    };
    operations[0x1000] = [&]()
    {
        pc = opcode & 0x0FFF;
    };
    operations[0x2000] = [&]()
    {
        stack.push(pc); // saving program counter(start point)
        pc = opcode & 0x0FFF; // Jump to address 0x0NNN

    };
    operations[0x3000] = [&]()
    {
        if( registers[reg_x] == (opcode & 0x00FF) ) 
            pc += 4; // skip instruction
        else
            pc += 2; // go to next instruction
    };
    operations[0x4000] = [&]()
    {
        if( registers[reg_x] != (opcode & 0x00FF) ) 
            pc += 4; // skip instruction
        else
            pc += 2; // go to next instruction

    };
    // Store number NN in VX
    // 0x6XNN
    operations[0x6000] = [&]()
    {
        registers[reg_x] = (opcode & 0x00FF);
        pc += 2; // next instruction
    };
    // add number NN to VX
    // 0x7XNN
    operations[0x7000] = [&]()
    {
        registers[reg_x] += (opcode & 0x00FF);
        pc += 2; // next instruction
    };
    operations[0x8000] = [&]()
    {
        operations[(((opcode & 0x000F) == 0x0000)? 0x8008 : (opcode & 0xF00F))]();
    };
    // Actually 0x8XY0
    // Just used 0x8008 to simplify
    operations[0x8008] = [&]()
    {
        registers[reg_x] = registers[reg_y];
        pc += 2;
    };
    operations[0x8001] = [&]()
    {
        registers[reg_x] |= registers[reg_y];
        pc += 2;
    };
    operations[0x8002] = [&]()
    {
        registers[reg_x] &= registers[reg_y];
        pc += 2;
    };
    operations[0x8003] = [&]()
    {
        registers[reg_x] ^= registers[reg_y];
        pc += 2;
    };
    operations[0x8004] = [&]()
    {
        if( registers[reg_y] > (0xFF - registers[reg_x]))
            registers[0xF] = 1;
        else 
            registers[0xF] = 0;

        registers[reg_x] += registers[reg_y];

        pc += 2;
    };
    operations[0x8005] = [&]()
    {
        if( registers[reg_y] > registers[reg_x])
            registers[0xF] = 0; // borrow
        else 
            registers[0xF] = 1;

        registers[reg_x] -= registers[reg_y];
        pc += 2;
    };
    operations[0x8006] = [&]()
    {
        registers[0xF] = registers[reg_x] & 0x1; // VF gets VX least significant bit
        registers[reg_x] >>= 1; // shift VX one bit to right 
        pc += 2;
    };
    operations[0x8007] = [&]()
    {
        if( registers[reg_y] > registers[reg_x])
            registers[0xF] = 0; // borrow
        else 
            registers[0xF] = 1;

        registers[reg_x] -= registers[reg_y];
        pc += 2;
    };
    operations[0x800E] = [&]()
    {
        registers[0xF] = registers[reg_x] >> 7; // VF gets VX most significant bit
        registers[reg_x] <<= 1; 
        pc += 2;
    };
    operations[0x9000] = [&]()
    {
        if( registers[reg_x] != registers[reg_y] )
            pc += 4; // skip instruction
        else
            pc += 2; // go to next instruction
    };
    operations[0xA000] = [&]()
    {
        I = opcode & 0x0FFF;
        pc += 2;
    };
    operations[0xB000] = [&]()
    {
        pc = (opcode & 0x0FFF) + registers[0];
    };
    operations[0xC000] = [&]()
    {
        registers[reg_x] = rand() & 0x00FF;
        pc += 2;
    };
    operations[0xD000] = [&]()
    {
        uint8_t x = registers[reg_x];
        uint8_t y = registers[reg_y];
        uint8_t height = opcode & 0x000F;
        uint8_t pixel;

        registers[0xF] = 0;
        for(int yline = 0; yline < height; yline++)
        {
            pixel = memory[I + yline];
            for(int xline = 0; xline < 8; xline++)
            {
                if((pixel & (0x80 >> xline)) != 0)
                {
                    if(graphics[(x+ xline + ((y+yline) * 64))] == 1)
                        registers[0xF] = 1;
                    graphics[(x+ xline + ((y+yline) * 64))] ^= 1;
                }
            }
        }

        draw_flag = true;
        pc += 2;
    };
    operations[0xE000] = [&]()
    {
        operations[opcode & 0xF0FF]();
    };
    operations[0xE09E] = [&]()
    {
        if( keyboard[ registers[reg_x] ] == true)  // key is pressed
            pc += 4;
        else 
            pc += 2;
    };
    operations[0xE0A1] = [&]()
    {
        if( keyboard[ registers[reg_x] ] == false)  // key not pressed
            pc += 4;
        else 
            pc += 2;
    };
    operations[0xF000] = [&]()
    {
        operations[opcode & 0xF0FF]();
    };
    operations[0xF007] = [&]()
    {
        registers[ reg_x ] = delay_timer;
        pc += 2;
    };
    operations[0xF00A] = [&]()
    {
        bool key_pressed = false;

        for(int i = 0; i < keyboard.size(); i++)
        {
            if(keyboard[i] != 0)
            {
                registers[reg_x] = i;
                key_pressed = true;
            }
        }

        if(!key_pressed)
            return;

        pc += 2;
    };
    operations[0xF015] = [&]()
    {
        delay_timer = registers[ reg_x ];
        pc += 2;
    };
    operations[0xF018] = [&]()
    {
        sound_timer = registers[ reg_x ];
        pc += 2;
    };
    operations[0xF01E] = [&]()
    {
        if( I + registers[ reg_x ] > 0xFFF)
            registers[0xF] = 1;
        else 
            registers[0xF] = 0;
        I += registers[ reg_x ];

        pc += 2;
    };
    operations[0xF029] = [&]()
    {
        I =  registers[reg_x] * 0x5;
        pc += 2;
    };
    operations[0xF033] = [&]()
    {
        memory[I] = registers[ reg_x ] / 100;
        memory[I + 1] = (registers[ reg_x ] / 10) % 10;
        memory[I + 2] = registers[ reg_x ]  % 10;
        pc += 2;
    };
    operations[0xF055] = [&]()
    {
        for( int i = 0; i <= reg_x; i++)
            memory[I + i] = registers[i];

        I += reg_x + 1;
        pc += 2;
    };
    operations[0xF065] = [&]()
    {
        for( int i = 0; i <= reg_x; i++)
            registers[i] =  memory[I + i];


        I += reg_x + 1;
        pc += 2;
    };
    




}


bool chip8::Core::load_ROM(std::string_view filename)
{
    init();

    std::cerr << filename.data() << '\n';
    const long pos = 0x200;
    std::ifstream input(filename.data(), std::ios::binary);
    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
    for (unsigned long i = 0; i < buffer.size(); i++)
    {
        memory[pos + i] = buffer[i];
    }
}


