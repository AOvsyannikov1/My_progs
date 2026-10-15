from collections import namedtuple
import array
import struct


BitField = namedtuple("BitField", ["size", "position"])


TYPE_CODES = {
    "char": 'c',
    "signed char": 'b',
    "unsigned char": 'B',
    "int8_t": 'b',
    "uint8_t": 'B',
    "bool": '?',

    "int16_t": 'h',
    "uint16_t": 'H',
    "short": 'h',
    "signed short": 'h',
    "unsigned short": 'H',

    "int32_t": "i",
    "uint32_t": "I",
    "int": 'i',
    "signed int": 'i',
    "unsigned int": 'i',

    "int64_t": 'l',
    "uint64_t": "L",
    "long": "l",
    "signed long": "l",
    "unsigned long": "L",

    "int128_t": 'q',
    "uint128_t": 'Q',
    "ssize_t": 'n',
    "size_t": 'N',
    "float": 'f',
    "double": 'd',
    "char[]": 's'
}


class StructDecoder:

    def __init__(self, c_struct_str: str, byte_order=""):
        struct_content = c_struct_str[c_struct_str.find('{') + 1:c_struct_str.find('}')].strip()

        object.__setattr__(self, "_StructDecoder__name", c_struct_str[c_struct_str.find('}')+1:].strip()[:-1])
        object.__setattr__(self, "_StructDecoder__bit_fields", dict())
        object.__setattr__(self, "_StructDecoder__decode_str", byte_order)
        object.__setattr__(self, "_StructDecoder__unnamed_fields_counter", 0)
        object.__setattr__(self, "_StructDecoder__indexes", dict())

        bit_position = 0
        var_index = 0
        for field in struct_content.split(';')[:-1]:
            field = field.strip()
            if "//" in field:
                field = field.split('\n')[-1].strip()
            # битовое поле
            if ':' in field:
                bit_position += self.__parse_bit_field(field, bit_position, var_index)
                if bit_position % 8 == 0:
                    if bit_position == 8:
                        self.__decode_str += "B"
                    elif bit_position == 16:
                        self.__decode_str += "H"
                    elif bit_position == 32:
                        self.__decode_str += "I"
                    elif bit_position == 64:
                        self.__decode_str += "L"
                    bit_position = 0
                    var_index += 1
            # переменная
            else:
                name = field.split()[-1]
                var_type = ' '.join(field.split()[:-1])

                typecode = TYPE_CODES[var_type]
                bracket_index = name.find("[")
                if bracket_index > 0:
                    array_size = int(name[bracket_index + 1:name.find(']')])
                    name = name[:bracket_index]
                    arr = array.array(typecode, [0] * array_size)
                    object.__setattr__(self, name, arr)
                    typecode *= array_size

                    self.__indexes[name] = var_index
                    var_index += array_size
                else:
                    if var_type == "char":
                        val = bytes(1)
                    else:
                        val = 0
                    self.__indexes[name] = var_index
                    var_index += 1

                    object.__setattr__(self, name, val)
                self.__decode_str += typecode

        object.__setattr__(self, "_StructDecoder__struct", struct.Struct(self.__decode_str))

    def __parse_bit_field(self, field, bit_position, var_index):
        type_and_name, size = field.split(':')
        type_and_name = type_and_name.split()

        size = int(size.strip())
        bit_field = BitField(size, bit_position)
        if len(type_and_name) == 2:
            name = type_and_name[-1]
        else:
            name = f"__UNNAMED_FIELD_{self.__unnamed_fields_counter}"
            self.__unnamed_fields_counter += 1
        object.__setattr__(self, name, 0)
        self.__bit_fields[name] = bit_field

        self.__indexes[name] = var_index

        return size

    def get(self, key):
        return self.__dict__[key]

    def size(self) -> int:
        return self.__struct.size
    
    def __str__(self):
        buf = f"{self.__name} \n{{\n"

        for key, val in self.__dict__.items():
            if key.find("__") < 0:
                buf += f"\t{key} = {val};\n"

        buf += "}\n"
        return buf
    
    def to_dict(self):
        res = {}
        for key, val in self.__dict__.items():
            if key.find("__") < 0:
                if type(val) == array.array:
                    res[key] = list(val)
                else:
                    res[key] = val
        return res

    def encode(self) -> bytes:
        values = list()
        bit_field_value = 0
        bit_field_size = 0

        for name in self.__dict__:
            if name.startswith("_StructDecoder__"):
                continue

            value = getattr(self, name)
            if name in self.__bit_fields.keys():        # битовое поле
                field_info = self.__bit_fields[name]

                bit_field_size += field_info.size
                bit_field_value += value << field_info.position
                if (field_info.position + field_info.size) % 8 == 0:
                    values.append(bit_field_value)
                    bit_field_value = 0
                    bit_field_size = 0
            else:
                if bit_field_size > 0:
                    values.append(bit_field_value)
                    bit_field_value = 0
                    bit_field_size = 0
                if type(value) == array.array:
                    for val in value:
                        values.append(val)
                else:
                    values.append(value)
        res = self.__struct.pack(*values)
        return res

    def decode(self, b: bytes):
        unpacked_struct = self.__struct.unpack(b)

        res_index = 0
        prev_field = False
        for name in self.__dict__:
            if name.startswith("_StructDecoder__"):
                continue

            if name in self.__bit_fields.keys():
                field_info = self.__bit_fields[name]

                prev_field = True
                val = unpacked_struct[res_index]

                mask = 0
                for i in range(field_info.size):
                    mask |= (1 << i)
                val = (val >> field_info.position) & mask
                setattr(self, name, val)
                if (field_info.position + field_info.size) % 8 == 0:
                    res_index += 1
                    prev_field = False
            else:
                if prev_field:
                    res_index += 1
                    prev_field = False
                attr = getattr(self, name)
                if type(attr) == array.array:
                    for i in range(len(attr)):
                        attr[i] = unpacked_struct[res_index]
                        res_index += 1
                else:
                    setattr(self, name, unpacked_struct[res_index])
                    res_index += 1

    def decode_one_field(self, b: bytes, name: str):
        unpacked_struct = self.__struct.unpack(b)
        var_index = self.__indexes[name]

        if name in self.__bit_fields:
            field_info = self.__bit_fields[name]
            val = unpacked_struct[var_index]

            mask = 0
            for i in range(field_info.size):
                mask |= (1 << i)
            val = (val >> field_info.position) & mask
            setattr(self, name, val)
            return val
        else:
            attr = getattr(self, name)

            if type(attr) == array.array:
                for i in range(len(attr)):
                    attr[i] = unpacked_struct[var_index]
                    var_index += 1
                return attr
            else:
                setattr(self, name, unpacked_struct[var_index])
                return unpacked_struct[var_index]

    def __setattr__(self, key, value):
        if not hasattr(self, key):
            raise AttributeError("Создание атрибутов запрещено!")
        else:
            object.__setattr__(self, key, value)

    @property
    def fmt(self):
        return self.__decode_str


if __name__ == "__main__":
    st = """
    typedef struct
    {
        uint8_t id;
        unsigned char carriage_id;
        unsigned short train_number;
        
        uint16_t pressures[20];
        uint16_t received_target;
        
        unsigned krrsh_B3307 :2;
        unsigned krrsh_B5008 :2;
        unsigned krrsh_B5017 :2;
        unsigned krrsh_B5020 :2;
        
        unsigned krrsh_B5509 :2;
        unsigned krrsh_B5004 :2;
        unsigned krrsh_B3306 :2;
        unsigned krrsh_L3305 :2;
        
        unsigned krrsh_B3301 :2;
        unsigned krrsh_B3302 :2;
        unsigned krrsh_B3303 :2;
        unsigned krrsh_B3304 :2;
        
        unsigned krrsh_B2810 :2;
        unsigned krrsh_B2812 :2;
        unsigned kvit_v1    :1;
        unsigned kvit_v2    :1;
        unsigned kvit_v3    :1;
        unsigned kvit_v4    :1;
        
        unsigned kvit_bust_on :1;
        unsigned kvit_bust_off :1;
        unsigned kvit_press_limit :1;
        unsigned kvit_bust_mode :1;
        unsigned kvit_vo :1;
        unsigned kvit_vt :1;
        unsigned kvit_vb :1;
        unsigned kvit_vtb :1;
        
        unsigned err_vlv_bust_on :1;
        unsigned err_vlv_bust_off :1;
        unsigned err_vlv_press_limit :1;
        unsigned err_vlv_bust_mode :1;
        unsigned err_vo :1;
        unsigned err_vt :1;
        unsigned err_vb :1;
        unsigned error_keb :1;
        
        unsigned kvit_keb   :1;
        unsigned emrg_valve_on  :1; 
        unsigned bust_state :1;
        unsigned bust_mode  :1;
        unsigned press_limitation_on  :1;
        unsigned bust_error           :1;
        unsigned ept_error            :1;
        unsigned vr_error             :1;
        
        unsigned pm_low_pressure      :1;
        unsigned pnr_low_pressure     :1;
        unsigned rd1_error      :1;
        unsigned rd2_error      :1;
        unsigned krrsh_combination_error  :1;
        unsigned butp1_connection         :1;
        unsigned butp2_connection         :1;
        unsigned reference_axle_valve       :1;
        
        unsigned kaet_pressed   :1;
        unsigned cmd_status     :2;
        unsigned emrg_loop_state :1;
        unsigned pet_voltage      :1;
        unsigned tc_pressure_source :1;
        unsigned vtb_error      :1;
        unsigned any_error      :1;
        
        unsigned logger_error         :1;
        unsigned reference_vlv_error    :1;
        unsigned spi_flash_error      :1;
        unsigned logging_active       :1;
        unsigned error_v1             :1;
        unsigned error_v2             :1;
        unsigned error_v3             :1;
        unsigned error_v4             :1;
        
        unsigned error_pk_b5016  :1;
        unsigned error_pk_b5506  :1;
        unsigned error_pk_b5307  :1;
        unsigned error_pk_b5013  :1;
        unsigned drive_error_b5008  :1;
        unsigned drive_error_b5004  :1;
        unsigned brake_released_loop  :1;
        unsigned parking_brake_released_loop  :1;
        
        uint8_t firmware_version;
        uint8_t firmware_subversion;
        uint8_t crc8;
    } bto_info_t;
    """

    struc = StructDecoder(st, '<')
