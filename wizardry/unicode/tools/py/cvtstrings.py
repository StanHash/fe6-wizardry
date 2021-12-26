import sys, re

def read_sjis_string(f):
    array = bytearray(b'')

    while True:
        byte = f.read(1)[0]

        if byte == 0:
            break

        array.append(byte)

    return array

def parse_string_locations(f):
    result = {}

    for line in f.readlines():
        line = line[:line.find("#")]
        line = line.strip()

        if len(line) == 0:
            continue

        m = re.match(r"(?P<addr>\w{8})\s+(?P<name>[a-zA-Z0-9_]+)", line)

        if m:
            addr = int(m.group('addr'), base = 16)
            name = m.group('name')
            result[addr] = f"String_{name}"

    return result

def main(args):
    try:
        rom_gba = args[1]
        str_txt = args[2]

    except IndexError:
        sys.exit(f"Usage: {args[0]} fe6.gba str.txt")

    with open(str_txt, 'r') as f:
        string_locations = parse_string_locations(f)

    with open(rom_gba, 'rb') as f:
        for str_addr in string_locations:
            str_name = string_locations[str_addr]

            f.seek(str_addr & 0x1FFFFFF)
            string = read_sjis_string(f).decode('cp932')

            print(f"    .global {str_name}")
            print(f"    .type {str_name}, object")
            print(f"{str_name}: @ '{string}'")
            print(f"    .asciz \"{string}\"")
            print(f"")

if __name__ == '__main__':
    main(sys.argv)
