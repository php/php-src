
"""GDB support for PHP types

This is auto-loaded by GDB if Python Auto-loading is enabled (the default), and
the PHP binary is in the auto load safe path:

    # ~/.config/gdb/gdbinit (not ~/.gdbinit)
    add-auto-load-safe-path /path/to/php-src

See https://sourceware.org/gdb/current/onlinedocs/gdb.html/Python-Auto_002dloading.html
See https://sourceware.org/gdb/current/onlinedocs/gdb.html/Auto_002dloading-safe-path.html#Auto_002dloading-safe-path

If needed, pretty printers can be by-passed by using the /r flag:
  (gdb) p /r any_variable

Use |set print pretty| to enable multi-line printing and indentation:
  (gdb) set print pretty on

Use |set print max-depth| to control the maximum print depth for nested
structures:
  (gdb) set print max-depth 5

To interactively type Python for development of the printers, use
  (gdb) python foo = gdb.parse_and_eval('bar')
to put the C value 'bar' in the current scope into a Python variable 'foo'.
Then you can interact with that variable:
  (gdb) python print foo['impl_']
"""

import gdb
import re
import traceback
import os

pp_set = gdb.printing.RegexpCollectionPrettyPrinter("php")

class ZendStringPrettyPrinter(gdb.printing.PrettyPrinter):
    "Print a zend_string"

    def __init__(self, val):
        self.val = val

    def to_string(self):
        return format_zstr(self.val)

    def children(self):
        for field in self.val.type.fields():
            if field.name == 'val':
                yield ('val', self.to_string())
            else:
                yield (field.name, self.val[field.name])


pp_set.add_printer('zend_string', '^_zend_string$', ZendStringPrettyPrinter)

def zendStringPointerPrinter(ptr):
    "Given a pointer to a zend_string, show the contents (if non-NULL)"
    if int(ptr) == 0:
        return '0x0'
    return format_zstr(ptr)

class ZendTypePrettyPrinter(gdb.printing.PrettyPrinter):
    "Print a zend_type"

    def __init__(self, val):
        self.val = val
        load_type_bits()

    def to_string(self):
        return self.format_type(self.val)

    def children(self):
        for field in self.val.type.fields():
            yield (field.name, self.val[field.name])

    def format_type(self, t):
        type_mask = int(t['type_mask'])
        type_mask_size = t['type_mask'].type.sizeof * 8
        separator = '|'
        parts = []
        meta = []
        for bit in range(0, type_mask_size):
            if type_mask & (1 << bit):
                type_name = type_bit_to_name.get(bit)
                match type_name:
                    case None:
                        parts.append('(1<<%d)' % bit)
                    case 'list':
                        list = t['ptr'].cast(gdb.lookup_type('zend_type_list').pointer())
                        num_types = int(list['num_types'])
                        types = list['types'].dereference().cast(gdb.lookup_type('zend_type').array(num_types))
                        for i in range(0, num_types):
                            str = self.format_type(types[i])
                            if any((c in set('|&()')) for c in str):
                                str = '(%s)' % str
                            parts.append(str)
                    case 'union' | 'arena':
                        meta.append(type_name)
                    case 'intersection':
                        meta.append(type_name)
                        separator = '&'
                    case 'name':
                        str = t['ptr'].cast(gdb.lookup_type('zend_string').pointer())
                        parts.append(format_zstr(str))
                    case _:
                        parts.append(type_name)

        str = separator.join(parts)

        if len(meta) > 0:
            str = '[%s] %s' % (','.join(meta), str)

        return str


pp_set.add_printer('zend_type', '^zend_type$', ZendTypePrettyPrinter)

class ZendAstKindPrettyPrinter(gdb.printing.PrettyPrinter):
    "Print a zend_ast_kind"

    def __init__(self, val):
        self.val = val

    def to_string(self):
        return self.val.cast(gdb.lookup_type('enum _zend_ast_kind'))


pp_set.add_printer('zend_ast_kind', '^zend_ast_kind$', ZendAstKindPrettyPrinter)

class ZendAstPrettyPrinter(gdb.printing.PrettyPrinter):
    "Print a zend_ast"

    def __init__(self, val):
        self.val = val

    def to_string(self):
        return '((%s*)0x%x)' % (str(self.cast().type), self.val.address)

    def children(self):
        val = self.cast()
        for field in val.type.fields():
            if field.name == 'child':
                children = val[field.name]
                num_children = self.num_children()

                ptr_type = gdb.lookup_type('zend_ast').pointer().pointer()
                children = children.cast(ptr_type)

                for i in range(0, num_children):
                    c = children[i]
                    if int(c) != 0:
                        c = c.dereference()
                    yield ('child[%d]' % i, c)
            elif field.name == 'name':
                yield (field.name, format_zstr(val[field.name]))
            elif field.name == 'val':
                yield (field.name, ZvalPrettyPrinter(val[field.name]).to_string())
            else:
                yield (field.name, val[field.name])

    def is_special(self):
        special_shift = 6 # ZEND_AST_SPECIAL_SHIFT
        kind = self.val['kind']
        return (kind >> special_shift) & 1

    def is_decl(self):
        return self.is_special() and int(self.val['kind']) >= enum_value('ZEND_AST_FUNC_DECL')

    def is_list(self):
        list_shift = 7 # ZEND_AST_IS_LIST_SHIFT
        kind = self.val['kind']
        return (kind >> list_shift) & 1

    def cast(self):
        kind = int(self.val['kind'])

        if kind == enum_value('ZEND_AST_ZVAL') or kind == enum_value('ZEND_AST_CONSTANT'):
            return self.val.cast(gdb.lookup_type('zend_ast_zval'))
        if kind == enum_value('ZEND_AST_ZNODE'):
            return self.val.cast(gdb.lookup_type('zend_ast_znode'))
        if self.is_decl():
            return self.val.cast(gdb.lookup_type('zend_ast_decl'))
        if self.is_list():
            return self.val.cast(gdb.lookup_type('zend_ast_list'))

        return self.val

    def num_children(self):
        if self.is_decl():
            decl_type = gdb.lookup_type('zend_ast_decl')
            child_type = decl_type['child'].type
            return array_size(child_type)
        if self.is_special():
            return 0
        elif self.is_list():
            return int(self.cast()['children'])
        else:
            num_children_shift = 8 # ZEND_AST_NUM_CHILDREN_SHIFT
            kind = self.val['kind']
            return kind >> num_children_shift


pp_set.add_printer('zend_ast', '^_zend_ast$', ZendAstPrettyPrinter)

class ZvalPrettyPrinter(gdb.printing.PrettyPrinter):
    "Print a zval"

    def __init__(self, val):
        self.val = val
        load_type_bits()

    def to_string(self):
        return self.value_to_string()

    def value_to_string(self):
        t = int(self.val['u1']['v']['type'])
        if t == type_name_to_bit['undef']:
            return 'undef'
        elif t == type_name_to_bit['null']:
            return 'null'
        elif t == type_name_to_bit['false']:
            return 'false'
        elif t == type_name_to_bit['true']:
            return 'true'
        elif t == type_name_to_bit['long']:
            return str(self.val['value']['lval'])
        elif t == type_name_to_bit['double']:
            return str(self.val['value']['dval'])
        elif t == type_name_to_bit['string']:
            return format_zstr(self.val['value']['str'])
        elif t == type_name_to_bit['array']:
            return 'array'
        elif t == type_name_to_bit['object']:
            return 'object(%s)' % format_zstr(self.val['value']['obj']['ce']['name'])
        elif t == type_name_to_bit['resource']:
            return 'resource'
        elif t == type_name_to_bit['reference']:
            return 'reference'
        elif t == type_name_to_bit['constant_ast']:
            return 'constant_ast'
        else:
            return 'zval of type %d' % int(self.val['u1']['v']['type'])

    def children(self):
        for field in self.val.type.fields():
            if field.name == 'value':
                value = self.val['value']
                t = int(self.val['u1']['v']['type'])
                if t == type_name_to_bit['undef']:
                    value = value['lval']
                elif t == type_name_to_bit['null']:
                    value = value['lval']
                elif t == type_name_to_bit['false']:
                    value = value['lval']
                elif t == type_name_to_bit['true']:
                    value = value['lval']
                elif t == type_name_to_bit['long']:
                    value = value['lval']
                elif t == type_name_to_bit['double']:
                    value = value['dval']
                elif t == type_name_to_bit['string']:
                    value = value['str'].dereference()
                elif t == type_name_to_bit['array']:
                    value = value['arr'].dereference()
                elif t == type_name_to_bit['object']:
                    value = value['obj'].dereference()
                elif t == type_name_to_bit['resource']:
                    value = value['res'].dereference()
                elif t == type_name_to_bit['reference']:
                    value = value['ref'].dereference()
                elif t == type_name_to_bit['constant_ast']:
                    value = value['ast'].dereference()
                else:
                    value = value['ptr']
                yield (field.name, value)
            elif field.name == 'u2':
                yield ('u2', self.val[field.name]['extra'])
            else:
                yield (field.name, self.val[field.name])


pp_set.add_printer('zval', '^_zval_struct$', ZvalPrettyPrinter)

class ZendClassEntryPrettyPrinter(gdb.printing.PrettyPrinter):
    "Print a zend_class_entry"

    # String pointers, show the string contents if possible
    STRING_FIELDS = [ 'name', 'doc_comment' ]

    def __init__(self, val):
        self.val = val

    def to_string(self):
        return zendStringPointerPrinter(self.val['name'])

    def children(self):
        for field in self.val.type.fields():
            if field.name is not None:
                if field.name in self.STRING_FIELDS:
                    yield (field.name, zendStringPointerPrinter(self.val[field.name]))
                elif field.name == 'ce_flags':
                    flags = self.val[field.name]
                    yield (field.name, '%d = %s' % (flags, ZendAccFlags.format_ce_flags(flags)))
                else:
                    yield (field.name, self.val[field.name])
            else:
                # Don't break on the union fields. Unfortunately, pretty
                # printers done in python cannot match the default formatting of
                # C anonymous fields, which omit the name entirely, see
                # binutils-gdb/gdb/cp-valprint.c#248 (as of commit
                # b6532accdd8e24329cc69bb58bc2883796008776)
                yield ('<anonymous>', self.val[field])

pp_set.add_printer('zend_class_entry', '^_zend_class_entry$', ZendClassEntryPrettyPrinter)

class ZendClassConstantPrettyPrinter(gdb.printing.PrettyPrinter):
    "Print a zend_class_constant"

    def __init__(self, val):
        self.val = val

    def children(self):
        for field in self.val.type.fields():
            if field.name == 'doc_comment':
                yield ('doc_comment', zendStringPointerPrinter(self.val['doc_comment']))
            elif field.name == 'ce':
                yield ('ce', zendStringPointerPrinter(self.val['ce']['name']))
            elif field.name == 'value':
                flags = self.val[field.name]['u2']['constant_flags']
                yield ('value.u2.constant_flags', '%d = %s' % (flags, ZendAccFlags.format_const_flags(flags)))
                yield (field.name, self.val[field.name])
            else:
                yield (field.name, self.val[field.name])

pp_set.add_printer('zend_class_constant', '^_zend_class_constant$', ZendClassConstantPrettyPrinter)

class ZendPropertyInfoPrettyPrinter(gdb.printing.PrettyPrinter):
    "Print a zend_property_info"

    def __init__(self, val):
        self.val = val

    def children(self):
        for field in self.val.type.fields():
            if field.name == 'name':
                yield (field.name, zendStringPointerPrinter(self.val[field.name]))
            elif field.name == 'flags':
                flags = self.val[field.name]
                yield ('flags', '%d = %s' % (flags, ZendAccFlags.format_prop_flags(flags)))
            else:
                yield (field.name, self.val[field.name])

pp_set.add_printer('zend_property_info', '^_zend_property_info$', ZendPropertyInfoPrettyPrinter)

class ZendFunctionPrettyPrinter(gdb.printing.PrettyPrinter):
    "Print a zend_function"

    def __init__(self, val):
        self.val = val

    def to_string(self):
        match int(self.val['type']):
            case ZendFnTypes.ZEND_INTERNAL_FUNCTION:
                typestr = 'internal'
            case ZendFnTypes.ZEND_USER_FUNCTION:
                typestr = 'user'
            case ZendFnTypes.ZEND_EVAL_CODE:
                typestr = 'eval'
            case _:
                typestr = '???'

        if self.val['common']['function_name']:
            namestr = format_zstr(self.val['common']['function_name'])
        else:
            namestr = '{main}'

        if self.val['common']['scope']:
            str = '%s method %s::%s' % (typestr, format_zstr(self.val['common']['scope']['name']), namestr)
        else:
            str = '%s function %s' % (typestr, namestr)

        if int(self.val['type']) == ZendFnTypes.ZEND_USER_FUNCTION or int(self.val['type']) == ZendFnTypes.ZEND_EVAL_CODE:
            str = '%s %s:%d' % (str, format_zstr(self.val['op_array']['filename']), int(self.val['op_array']['line_start']))

        return str

    def children(self):
        for field in self.val.type.fields():
            if field.name == 'common':
                continue
            elif field.name == 'op_array':
                if int(self.val['type']) == ZendFnTypes.ZEND_USER_FUNCTION or int(self.val['type']) == ZendFnTypes.ZEND_EVAL_CODE:
                    yield (field.name, self.val[field.name])
            elif field.name == 'internal_function':
                if int(self.val['type']) == ZendFnTypes.ZEND_INTERNAL_FUNCTION:
                    yield (field.name, self.val[field.name])
            else:
                yield (field.name, self.val[field.name])

pp_set.add_printer('zend_function', '^_zend_function$', ZendFunctionPrettyPrinter)

class ZendOpArrayPrettyPrinter(gdb.printing.PrettyPrinter):
    "Print a zend_op_array"

    def __init__(self, val):
        self.val = val

    def to_string(self):
        if self.val['function_name']:
            namestr = format_zstr(self.val['function_name'])
        else:
            namestr = '{main}'

        if self.val['scope']:
            str = 'method %s::%s' % (format_zstr(self.val['scope']['name']), namestr)
        else:
            str = 'function %s' % (namestr)

        str = '%s %s:%d' % (str, format_zstr(self.val['filename']), int(self.val['line_start']))

        return str

    def children(self):
        for field in self.val.type.fields():
            if field.name == 'fn_flags':
                value = self.val[field.name]
                yield (field.name, '%d = %s' % (value, ZendAccFlags.format_fn_flags(value)))
            else:
                yield (field.name, self.val[field.name])

pp_set.add_printer('zend_op_array', '^_zend_op_array$', ZendOpArrayPrettyPrinter)

class ZendOpPrettyPrinter(gdb.printing.PrettyPrinter):
    "Print a zend_op"

    def __init__(self, val):
        self.val = val

    def children(self):
        for field in self.val.type.fields():
            if field.name == 'opcode':
                opcode = int(self.val[field.name])
                yield (field.name, '%d = %s' % (opcode, ZendOpcodes.name(opcode)))
            else:
                yield (field.name, self.val[field.name])

pp_set.add_printer('zend_op', '^_zend_op$', ZendOpPrettyPrinter)

class ZendInternalFunctionPrettyPrinter(gdb.printing.PrettyPrinter):
    "Print a zend_internal_function"

    def __init__(self, val):
        self.val = val

    def to_string(self):
        if self.val['function_name']:
            namestr = format_zstr(self.val['function_name'])
        else:
            namestr = '???'

        if self.val['scope']:
            str = 'method %s::%s' % (format_zstr(self.val['scope']['name']), namestr)
        else:
            str = 'function %s' % (namestr)

        return str

    def children(self):
        for field in self.val.type.fields():
            if field.name == 'fn_flags':
                yield ('fn_flags', ('%d = %s' % (self.val[field.name], ZendAccFlags.format_fn_flags(self.val[field.name]))))
            else:
                yield (field.name, self.val[field.name])

pp_set.add_printer('zend_internal_function', '^_zend_internal_function$', ZendInternalFunctionPrettyPrinter)

class PrintAccFlagsCommand(gdb.Command):
    "Pretty print ACC flags"

    def __init__ (self, type):
        self.type = type
        name = 'print_%s_flags' % type
        super(PrintAccFlagsCommand, self).__init__(name, gdb.COMMAND_USER)

    def invoke (self, arg, from_tty):
        print(ZendAccFlags.format_flags(arg, self.type))

PrintAccFlagsCommand('fn')
PrintAccFlagsCommand('ce')
PrintAccFlagsCommand('prop')
PrintAccFlagsCommand('const')

class PrintOpcodeCommand(gdb.Command):
    "Pretty print opcode"

    def __init__ (self):
        super(PrintOpcodeCommand, self).__init__("print_opcode", gdb.COMMAND_USER)

    def invoke (self, arg, from_tty):
        print(ZendOpcodes.name(arg))

PrintOpcodeCommand()

type_bit_to_name = None
type_name_to_bit = None

def load_type_bits():
    global type_bit_to_name
    global type_name_to_bit

    if type_bit_to_name != None:
        return

    dirname = detect_source_dir()
    filename = os.path.join(dirname, 'zend_types.h')

    bits = {}

    with open(filename, 'r') as file:
        content = file.read()

        pattern = re.compile(r'#define _ZEND_TYPE_([^\s]+)_BIT\s+\(1u << (\d+)\)')
        matches = pattern.findall(content)
        for name, bit in matches:
            bits[int(bit)] = name.lower()

        pattern = re.compile(r'#define IS_([^\s]+)\s+(\d+)')
        matches = pattern.findall(content)
        for name, bit in matches:
            if not int(bit) in bits:
                bits[int(bit)] = name.lower()

    types = {}
    for bit in bits:
        types[bits[bit]] = bit

    type_bit_to_name = bits
    type_name_to_bit = types

class ZendFnTypes:
    ZEND_INTERNAL_FUNCTION = 1
    ZEND_USER_FUNCTION = 2
    ZEND_EVAL_CODE = 4

class ZendAccFlag:
    ce = False
    fn = False
    prop = False
    const = False
    bit = 0
    def __init__(self, ce, fn, prop, const, bit):
        self.ce = ce
        self.fn = fn
        self.prop = prop
        self.const = const
        self.bit = bit

    def applies_to(self, type):
        return getattr(self, type)

class ZendAccFlags:
    _flags = None

    @classmethod
    def fn_flag_name(self, bit):
        self.flag_name(bit, 'fn')

    @classmethod
    def ce_flag_name(self, bit):
        self.flag_name(bit, 'ce')

    @classmethod
    def prop_flag_name(self, bit):
        self.flag_name(bit, 'prop')

    @classmethod
    def const_flag_name(self, bit):
        self.flag_name(bit, 'const')

    @classmethod
    def flag_name(self, bit, type):
        self._load()
        for name in self._flags:
            flag = self._flags[name]
            if flag.applies_to(type) and bit == flag.bit:
                return name

    @classmethod
    def flag_bit(self, name):
        self._load()
        return self._flags[name]

    @classmethod
    def format_flags(self, flags, type):
        flags = int(flags)
        names = []
        for i in range(0, 31):
            if (flags & (1 << i)) != 0:
                name = self.flag_name(i, type)
                if name == None:
                    names.append('(1 << %d)' % (i))
                else:
                    names.append(name)
        return ' | '.join(names)

    @classmethod
    def format_fn_flags(self, flags):
        return self.format_flags(flags, 'fn')

    @classmethod
    def format_ce_flags(self, flags):
        return self.format_flags(flags, 'ce')

    @classmethod
    def format_prop_flags(self, flags):
        return self.format_flags(flags, 'prop')

    @classmethod
    def format_const_flags(self, flags):
        return self.format_flags(flags, 'const')

    @classmethod
    def _load(self):
        if self._flags != None:
            return

        dirname = detect_source_dir()
        filename = os.path.join(dirname, 'zend_compile.h')

        flags = {}

        with open(filename, 'r') as file:
            content = file.read()

            pattern = re.compile(r'#define (ZEND_ACC_[^\s]+)\s+\(1U?\s+<<\s+(\d+)\)\s+/\*\s+(X?)\s+\|\s+(X?)\s+\|\s+(X?)\s+\|\s+(X?)\s+\*/')
            matches = pattern.findall(content)
            for name, bit, cls, func, prop, const in matches:
                flags[name] = ZendAccFlag(cls == 'X', func == 'X', prop == 'X', const == 'X', int(bit))

        self._flags = flags

class ZendOpcodes:
    _opcodes = None

    @classmethod
    def name(self, number):
        self._load()
        number = int(number)
        for name in self._opcodes:
            if number == self._opcodes[name]:
                return name

    @classmethod
    def number(self, name):
        self._load()
        return self._opcodes[name]

    @classmethod
    def _load(self):
        if self._opcodes != None:
            return

        dirname = detect_source_dir()
        filename = os.path.join(dirname, 'zend_vm_opcodes.h')

        opcodes = {}

        with open(filename, 'r') as file:
            content = file.read()

            pattern = re.compile(r'#define (ZEND_[^\s]+)\s+([0-9]+)')
            matches = pattern.findall(content)
            for name, number in matches:
                if name == 'ZEND_VM_LAST_OPCODE':
                    continue
                opcodes[name] = int(number)

        self._opcodes = opcodes

def detect_source_dir():
    (symbol,_) = gdb.lookup_symbol("zend_visibility_to_set_visibility")
    if symbol == None:
        raise Exception("Could not find zend_compile.h: symbol zend_visibility_to_set_visibility not found")
    filename = symbol.symtab.fullname()
    dirname = os.path.dirname(filename)
    return dirname

def lookup_symbol(name):
    (symbol, _) = gdb.lookup_symbol(name)
    if symbol == None:
        raise Exception("Could not lookup symbol %s" % name)
    return symbol

def enum_value(name):
    symbol = lookup_symbol(name)
    return int(symbol.value())

def array_size(ary_type):
    # array types have a single field whose type represents its range
    return ary_type.fields()[0].type.range()[1]+1

def format_zstr(zstr):
    len = int(zstr['len'])
    truncated = False
    if len > 200:
        len = 200
        truncated = True

    ptr_type = gdb.lookup_type('char').pointer()
    ary_type = gdb.lookup_type('char').array(len)
    str = zstr['val'].cast(ptr_type).dereference().cast(ary_type)
    str = str.format_string()
    if truncated:
        str += ' (%d bytes total)' % int(zstr['len'])

    return str

gdb.printing.register_pretty_printer(gdb, pp_set, replace=True)
