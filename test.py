
target = "bcm2710-rpi-3-b-plus.dtb"


def read_binary_file_as_bits(file_path):
    bits = []
    with open(file_path, 'rb') as file:
        while byte := file.read(1):  # Read one byte at a time
            # Convert byte to its binary representation, strip the '0b' prefix, and pad to 8 bits
            bits.append(format(ord(byte), '08b'))
    # Join all bits into a single string
    return ''.join(bits)

# Example usage
binary_bits = read_binary_file_as_bits(target)
print(binary_bits)


