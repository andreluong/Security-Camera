#include "i2cHelpers.h"


namespace i2cOperations {
    int init_i2c_bus(const char* bus, int address)
    {
        int i2c_file_desc = open(bus, O_RDWR);
        if (i2c_file_desc == -1) {
            printf("I2C DRV: Unable to open bus for read/write (%s)\n", bus);
            perror("Error is:");
            exit(EXIT_FAILURE);
        }
        if (ioctl(i2c_file_desc, I2C_SLAVE, address) == -1) {
            perror("Unable to set I2C device to slave address.");
            exit(EXIT_FAILURE);
        }
        return i2c_file_desc;
    }

    void write_i2c_reg16(int i2c_file_desc, uint8_t reg_addr, uint16_t value)
    {
        int tx_size = 1 + sizeof(uint16_t);
        uint8_t buff[BUFFER_SIZE];
        buff[0] = reg_addr;
        buff[1] = (value & 0xFF);
        buff[2] = (value & 0xFF00) >> 8;
        int bytes_written = write(i2c_file_desc, buff, tx_size);
        if (bytes_written != tx_size) {
            perror("Unable to write i2c register");
            exit(EXIT_FAILURE);
        }
    }

    uint16_t read_i2c_reg16(int i2c_file_desc, uint8_t reg_addr)
    {
        // To read a register, must first write the address
        int bytes_written = write(i2c_file_desc, &reg_addr, sizeof(reg_addr));
        if (bytes_written != sizeof(reg_addr)) {
            perror("Unable to write i2c register.");
            exit(EXIT_FAILURE);
        }
        // Now read the value and return it
        uint16_t value = 0;
        int bytes_read = read(i2c_file_desc, &value, sizeof(value));
        if (bytes_read != sizeof(value)) {
            perror("Unable to read i2c register");
            exit(EXIT_FAILURE);
        }
        return value;
    }

    //Turn the raw numbers read from ADC into a useable format
    uint16_t swap_and_scale(uint16_t value) {
        uint16_t swap = ((value & 0xFF00) >> 8 | (value & 0x00FF) << 8);
        uint16_t scaled_read = swap >> 4;
        return scaled_read;
    }
}

