
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
import gdb.printing
import gdb.types
import re
import traceback
import os

pp_set = gdb.printing.RegexpCollectionPrettyPrinter("php")

class ZendStringPrettyPrinter(gdb.printing.PrettyPrinter):
    "Print a zend_string"

    def __init__(self, val):
        self.val = val

    def to_string(self):
        return '((zend_string*) 0x%x) %s' % (self.val.address, format_zstr(self.val))

    def children(self):
        for field in self.val.type.fields():
            if field.name == 'val':
                yield ('val', format_zstr(self.val))
            else:
                yield (field.name, format_nested(self.val[field.name]))

pp_set.add_printer('zend_string', '^_zend_string$', ZendStringPrettyPrinter)

class ZendObjectPrettyPrinter(gdb.printing.PrettyPrinter):
    "Print a zend_object"

    def __init__(self, val):
        self.val = val

    def to_string(self):
        return 'object(%s)' % format_zstr(self.val['ce']['name'])

    def children(self):
        for field in self.val.type.fields():
            yield (field.name, format_nested(self.val[field.name]))

pp_set.add_printer('zend_object', '^_zend_object$', ZendObjectPrettyPrinter)

class ZendArrayPrettyPrinter(gdb.printing.PrettyPrinter):
    "Print a zend_array"

    def __init__(self, val):
        self.val = val

    def to_string(self):
        return 'array(%d)' % self.val['nNumOfElements']

    def children(self):
        for field in self.val.type.fields():
            if field.name is None:
                name = '<anonymous>'
                val = self.val[field]
            else:
                name = field.name
                val = self.val[field.name]
            yield (name, format_nested(val))

pp_set.add_printer('zend_array', '^_zend_array$', ZendArrayPrettyPrinter)

class ZendTypePrettyPrinter(gdb.printing.PrettyPrinter):
    "Print a zend_type"

    def __init__(self, val):
        self.val = val

    def to_string(self):
        return '((zend_type*) 0x%x) %s' % (self.val.address, self.format_type(self.val))

    def children(self):
        for field in self.val.type.fields():
            yield (field.name, format_nested(self.val[field.name]))

    def format_type(self, t):
        type_mask = int(t['type_mask'])
        type_mask_size = t['type_mask'].type.sizeof * 8
        separator = '|'
        parts = []
        meta = []
        for bit in range(0, type_mask_size):
            if type_mask & (1 << bit):
                type_name = ZendTypeBits.zendTypeName(bit)
                if type_name is None:
                    parts.append('(1<<%d)' % bit)
                elif type_name == 'list':
                    list_ptr = t['ptr'].cast(gdb.lookup_type('zend_type_list').pointer())
                    num_types = int(list_ptr['num_types'])
                    types = list_ptr['types'].dereference().cast(gdb.lookup_type('zend_type').array(num_types))
                    for i in range(0, num_types):
                        type_str = self.format_type(types[i])
                        if any((c in set('|&()')) for c in type_str):
                            type_str = '(%s)' % type_str
                        parts.append(type_str)
                elif type_name == 'union' or type_name == 'arena':
                    meta.append(type_name)
                elif type_name == 'intersection':
                    meta.append(type_name)
                    separator = '&'
                elif type_name == 'name':
                    name_str = t['ptr'].cast(gdb.lookup_type('zend_string').pointer())
                    parts.append(format_zstr(name_str))
                else:
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
    """
        Print a zend_ast, or one of the specialized structures based on it:
        zend_ast_decl, zend_ast_list, zend_ast_op_array, zend_ast_zval, or
        zend_ast_znode
    """

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
            elif field.name == 'val':
                yield (field.name, ZvalPrettyPrinter(val[field.name]).to_string())
            else:
                yield (field.name, format_nested(val[field.name]))

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
        if kind == enum_value('ZEND_AST_OP_ARRAY'):
            return self.val.cast(gdb.lookup_type('zend_ast_op_array'))
        if kind == enum_value('ZEND_AST_CALLABLE_CONVERT'):
            return self.val.cast(gdb.lookup_type('zend_ast_fcc'))
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
pp_set.add_printer('zend_ast_decl', '^_zend_ast_decl$', ZendAstPrettyPrinter)
pp_set.add_printer('zend_ast_list', '^_zend_ast_list$', ZendAstPrettyPrinter)
pp_set.add_printer('zend_ast_op_array', '^_zend_ast_op_array$', ZendAstPrettyPrinter)
pp_set.add_printer('zend_ast_fcc', '^_zend_ast_fcc$', ZendAstPrettyPrinter)
pp_set.add_printer('zend_ast_zval', '^_zend_ast_zval$', ZendAstPrettyPrinter)
pp_set.add_printer('zend_ast_znode', '^_zend_ast_znode$', ZendAstPrettyPrinter)

class ZvalPrettyPrinter(gdb.printing.PrettyPrinter):
    "Print a zval"

    def __init__(self, val):
        self.val = val

    def to_string(self):
        return '((zval*) 0x%x) %s' % (self.val.address, self.value_to_string())

    def value_to_string(self):
        t = int(self.val['u1']['v']['type'])
        if t == ZendTypeBits.bit('undef'):
            return 'undef'
        elif t == ZendTypeBits.bit('null'):
            return 'null'
        elif t == ZendTypeBits.bit('false'):
            return 'false'
        elif t == ZendTypeBits.bit('true'):
            return 'true'
        elif t == ZendTypeBits.bit('long'):
            return str(self.val['value']['lval'])
        elif t == ZendTypeBits.bit('double'):
            return str(self.val['value']['dval'])
        elif t == ZendTypeBits.bit('string'):
            return format_zstr(self.val['value']['str'])
        elif t == ZendTypeBits.bit('array'):
            return 'array'
        elif t == ZendTypeBits.bit('object'):
            return 'object(%s)' % format_zstr(self.val['value']['obj']['ce']['name'])
        elif t == ZendTypeBits.bit('resource'):
            return 'resource'
        elif t == ZendTypeBits.bit('reference'):
            return 'reference'
        elif t == ZendTypeBits.bit('constant_ast'):
            return 'constant_ast'
        elif t == ZendTypeBits.bit('indirect'):
            value = self.val['value']['zv']
            valuestr = ZvalPrettyPrinter(value).to_string()
            return 'indirect: ((zval*) 0x%x) %s' % (int(value), valuestr)
        elif t == ZendTypeBits.bit('ptr'):
            value = int(self.val['value']['ptr'])
            return 'ptr: ((void*) 0x%x)' % (value)
        elif t == ZendTypeBits.bit('alias_ptr'):
            value = int(self.val['value']['ptr'])
            return 'alias_ptr: ((void*) 0x%x)' % (value)
        else:
            return 'zval of type %d' % int(self.val['u1']['v']['type'])

    def children(self):
        for field in self.val.type.fields():
            if field.name == 'value':
                value = self.val['value']
                sub_field = 'ptr'
                t = int(self.val['u1']['v']['type'])
                if t == ZendTypeBits.bit('undef'):
                    sub_field = 'lval'
                elif t == ZendTypeBits.bit('null'):
                    sub_field = 'lval'
                elif t == ZendTypeBits.bit('false'):
                    sub_field = 'lval'
                elif t == ZendTypeBits.bit('true'):
                    sub_field = 'lval'
                elif t == ZendTypeBits.bit('long'):
                    sub_field = 'lval'
                elif t == ZendTypeBits.bit('double'):
                    sub_field = 'dval'
                elif t == ZendTypeBits.bit('string'):
                    sub_field = 'str'
                elif t == ZendTypeBits.bit('array'):
                    sub_field = 'arr'
                elif t == ZendTypeBits.bit('object'):
                    sub_field = 'obj'
                elif t == ZendTypeBits.bit('resource'):
                    sub_field = 'res'
                elif t == ZendTypeBits.bit('reference'):
                    sub_field = 'ref'
                elif t == ZendTypeBits.bit('constant_ast'):
                    sub_field = 'ast'
                elif t == ZendTypeBits.bit('indirect'):
                    sub_field = 'zv'
                value = value[sub_field]
                if sub_field != 'ptr' and value.type.code == gdb.TYPE_CODE_PTR:
                    value = value.dereference()
                yield ('%s.%s' % (field.name, sub_field), value)
            elif field.name == 'u2':
                yield ('u2', self.val[field.name]['extra'])
            else:
                yield (field.name, format_nested(self.val[field.name]))


pp_set.add_printer('zval', '^_zval_struct$', ZvalPrettyPrinter)

class ZendClassEntryPrettyPrinter(gdb.printing.PrettyPrinter):
    "Print a zend_class_entry"

    def __init__(self, val):
        self.val = val

    def to_string(self):
        return '((zend_class_entry*) 0x%x) %s' % (self.val.address, format_zstr(self.val['name']))

    def children(self):
        for field in self.val.type.fields():
            if field.name is not None:
                if field.name == 'ce_flags':
                    flags = self.val[field.name]
                    yield (field.name, '%d (%s)' % (flags, ZendAccFlags.format_ce_flags(flags)))
                else:
                    yield (field.name, format_nested(self.val[field.name]))
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
            if field.name == 'value':
                flags = self.val[field.name]['u2']['constant_flags']
                yield ('value.u2.constant_flags', '%d (%s)' % (flags, ZendAccFlags.format_const_flags(flags)))
                yield (field.name, self.val[field.name])
            else:
                yield (field.name, format_nested(self.val[field.name]))

pp_set.add_printer('zend_class_constant', '^_zend_class_constant$', ZendClassConstantPrettyPrinter)

class ZendPropertyInfoPrettyPrinter(gdb.printing.PrettyPrinter):
    "Print a zend_property_info"

    def __init__(self, val):
        self.val = val

    def children(self):
        for field in self.val.type.fields():
            if field.name == 'flags':
                flags = self.val[field.name]
                yield ('flags', '%d (%s)' % (flags, ZendAccFlags.format_prop_flags(flags)))
            else:
                yield (field.name, format_nested(self.val[field.name]))

pp_set.add_printer('zend_property_info', '^_zend_property_info$', ZendPropertyInfoPrettyPrinter)

class ZendFunctionPrettyPrinter(gdb.printing.PrettyPrinter):
    "Print a zend_function"

    def __init__(self, val):
        self.val = val

    def to_string(self):
        val_type = int(self.val['type'])
        if val_type == ZendFnTypes.ZEND_INTERNAL_FUNCTION:
            typestr = 'internal'
        elif val_type == ZendFnTypes.ZEND_USER_FUNCTION:
            typestr = 'user'
        elif val_type == ZendFnTypes.ZEND_EVAL_CODE:
            typestr = 'eval'
        else:
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

        return '((zend_function*) 0x%x) %s' % (self.val.address, str)

    def children(self):
        for field in self.val.type.fields():
            if field.name == 'common' or field.name == 'type' or field.name == 'quick_arg_flags':
                # Redundant with op_array / internal_function
                continue
            elif field.name == 'op_array':
                if int(self.val['type']) == ZendFnTypes.ZEND_USER_FUNCTION or int(self.val['type']) == ZendFnTypes.ZEND_EVAL_CODE:
                    yield (field.name, self.val[field.name])
            elif field.name == 'internal_function':
                if int(self.val['type']) == ZendFnTypes.ZEND_INTERNAL_FUNCTION:
                    yield (field.name, self.val[field.name])
            else:
                yield (field.name, format_nested(self.val[field.name]))

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

        return '((zend_op_array*) 0x%x) %s' % (self.val.address, str)

    def children(self):
        for field in self.val.type.fields():
            if field.name == 'fn_flags':
                value = self.val[field.name]
                yield (field.name, '%d (%s)' % (value, ZendAccFlags.format_fn_flags(value)))
            else:
                yield (field.name, format_nested(self.val[field.name]))

pp_set.add_printer('zend_op_array', '^_zend_op_array$', ZendOpArrayPrettyPrinter)

class ZendOpPrettyPrinter(gdb.printing.PrettyPrinter):
    "Print a zend_op"

    def __init__(self, val):
        self.val = val

    def children(self):
        for field in self.val.type.fields():
            if field.name == 'opcode':
                opcode = int(self.val[field.name])
                yield (field.name, '%d (%s)' % (opcode, ZendOpcodes.name(opcode)))
            else:
                yield (field.name, format_nested(self.val[field.name]))

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

        return '((zend_internal_function*) 0x%x) %s' % (self.val.address, str)

    def children(self):
        for field in self.val.type.fields():
            if field.name == 'fn_flags':
                yield ('fn_flags', ('%d (%s)' % (self.val[field.name], ZendAccFlags.format_fn_flags(self.val[field.name]))))
            else:
                yield (field.name, format_nested(self.val[field.name]))

pp_set.add_printer('zend_internal_function', '^_zend_internal_function$', ZendInternalFunctionPrettyPrinter)

class ZendRefcountedHPrettyPrinter(gdb.printing.PrettyPrinter):
    "Print a zend_refcounted_h"

    def __init__(self, val):
        self.val = val

    def children(self):
        for field in self.val.type.fields():
            if field.name == 'u':
                val = self.val[field.name]
                if val == None:
                    val = self.val
                for subfield in val.type.fields():
                    if subfield.name == 'type_info':
                        flags = int(val[subfield.name])
                        yield (('%s.%s' % (field.name, subfield.name)), '%d (%s)' % (flags, ZendRefTypeInfo.format(flags)))
                    else:
                        yield (('%s.%s' % (field.name, subfield.name)), format_nested(val[subfield.name]))
            else:
                yield (('%s' % field.name), format_nested(self.val[field.name]))

pp_set.add_printer('zend_refcounted_h', '^_zend_refcounted_h$', ZendRefcountedHPrettyPrinter)

class PrintAccFlagsCommand(gdb.Command):
    "Pretty print ACC flags"

    def __init__ (self, type):
        self.type = type
        name = 'print_%s_flags' % type
        super(PrintAccFlagsCommand, self).__init__(name, gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION)

    def invoke (self, arg, from_tty):
        arg = int(gdb.parse_and_eval(arg))
        print(ZendAccFlags.format_flags(arg, self.type))

PrintAccFlagsCommand('fn')
PrintAccFlagsCommand('ce')
PrintAccFlagsCommand('prop')
PrintAccFlagsCommand('const')

class PrintOpcodeCommand(gdb.Command):
    "Pretty print opcode"

    def __init__ (self):
        super(PrintOpcodeCommand, self).__init__("print_opcode", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION)

    def invoke (self, arg, from_tty):
        arg = int(gdb.parse_and_eval(arg))
        print(ZendOpcodes.name(arg))

PrintOpcodeCommand()

class PrintRefTypeInfoCommand(gdb.Command):
    "Pretty print zend_refcounted.gc.u.type_info"

    def __init__ (self):
        super(PrintRefTypeInfoCommand, self).__init__("print_ref_type_info", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION)

    def invoke (self, arg, from_tty):
        arg = int(gdb.parse_and_eval(arg))
        print(ZendRefTypeInfo.format(arg))

PrintRefTypeInfoCommand()

class DumpOpArrayCommand(gdb.Command):
    "Dump an op_array"

    def __init__ (self):
        super(DumpOpArrayCommand, self).__init__("dump_op_array", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION)

    def invoke (self, arg, from_tty):
        op_array = gdb.parse_and_eval(arg)
        if op_array.type.code != gdb.TYPE_CODE_PTR:
            print("Must pass a zend_op_array* (got a %s)" % op_array.type)
            return
        if str(gdb.types.get_basic_type(op_array.type.target())) != 'struct _zend_op_array':
            print("Must pass a zend_op_array* (got a %s)" % op_array.type)
            return
        if int(op_array) == 0:
            print("NULL")
            return
        gdb.execute("call zend_dump_op_array((zend_op_array*)0x%x, 0, 0, 0)" % (int(op_array)))
        return

DumpOpArrayCommand()

class ZendTypeBits:
    _bits = None

    @classmethod
    def zendTypeName(self, bit):
        self._load()
        for name in self._bits:
            if bit == self._bits[name]:
                return name

    @classmethod
    def zvalTypeName(self, bit):
        # Same as zendTypeName, but return the last matching one
        # e.g. 13 is IS_PTR, not IS_ITERABLE
        self._load()
        ret = None
        for name in self._bits:
            if bit == self._bits[name]:
                ret = name
        return ret

    @classmethod
    def bit(self, name):
        self._load()
        return self._bits.get(name)

    @classmethod
    def _load(self):
        if self._bits != None:
            return

        dirname = detect_source_dir()
        filename = os.path.join(dirname, 'zend_types.h')

        bits = {}

        with open(filename, 'r') as file:
            content = file.read()

            pattern = re.compile(r'#define _ZEND_TYPE_([^\s]+)_BIT\s+\(1u << (\d+)\)')
            matches = pattern.findall(content)
            for name, bit in matches:
                bits[name.lower()] = int(bit)

            pattern = re.compile(r'#define IS_([^\s]+)\s+(\d+)')
            matches = pattern.findall(content)
            for name, bit in matches:
                bits[name.lower()] = int(bit)

        self._bits = bits

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
        found_nop = False

        with open(filename, 'r') as file:
            content = file.read()

            pattern = re.compile(r'#define (ZEND_[^\s]+)\s+([0-9]+)')
            matches = pattern.findall(content)
            for name, number in matches:
                if not found_nop:
                    if name == 'ZEND_NOP':
                        found_nop = True
                    else:
                        continue
                if name == 'ZEND_VM_LAST_OPCODE':
                    break
                opcodes[name] = int(number)

        self._opcodes = opcodes

class ZendRefTypeInfo:
    _bits = None

    @classmethod
    def flag_name(self, bit):
        return self._flag_name_in(bit, self._bits)

    @classmethod
    def _flag_name_in(self, bit, bits):
        for name in bits:
            if bit == bits[name]:
                return name

    @classmethod
    def bit(self, name):
        return self._bits.get(name)

    @classmethod
    def format(self, flags):
        self._load()
        names = []

        type = flags & self._type_mask
        type_name = ZendTypeBits.zvalTypeName(type)
        if type_name is not None:
            names.append('IS_%s' % type_name.upper())

        bits = self._bits
        type_bits = None
        if type_name == 'string':
            type_bits = self._str_bits
        elif type_name == 'array':
            type_bits = self._array_bits
        elif type_name == 'object':
            type_bits = self._obj_bits

        type_flags = flags & self._flags_mask
        for i in range(0, 31):
            if (1<<i) > type_flags:
                break
            if (type_flags & (1<<i)) != 0:
                name = self.flag_name(i)
                if type_bits is not None:
                    name2 = self._flag_name_in(i, type_bits)
                    if name2 is not None:
                        if name is not None:
                            names.append('%s(%s)' % (name2, name))
                        else:
                            names.append(name2)
                        continue
                if name is not None:
                    names.append(name)
                else:
                    names.append('(1 << %d)' % (i))

        if (flags & (1<<self.bit('GC_NOT_COLLECTABLE'))) == 0:
            gc_color = (flags >> self._info_shift) & self._gc_color
            if gc_color == self._gc_black:
                names.append('GC_BLACK')
            elif gc_color == self._gc_white:
                names.append('GC_WHITE')
            elif gc_color == self._gc_grey:
                names.append('GC_GREY')
            elif gc_color == self._gc_purple:
                names.append('GC_PURPLE')

            gc_address = (flags >> self._info_shift) & self._gc_address
            if gc_address != 0:
                names.append('GC_ADDRESS(%d)' % gc_address)
        else:
            info = flags & self._info_mask
            if info != 0:
                names.append('GC_INFO(%d)' % info)

        return ' | '.join(names)

    @classmethod
    def _load(self):
        if self._bits != None:
            return

        dirname = detect_source_dir()
        filename = os.path.join(dirname, 'zend_types.h')

        bits = {}
        str_bits = {}
        array_bits = {}
        obj_bits = {}

        with open(filename, 'r') as file:
            content = file.read()

            # GC_NOT_COLLECTABLE          (1<<4)
            pattern = re.compile(r'#define (GC_[^\s]+)\s+\(\s*1\s*<<\s*([0-9]+)\s*\)')
            matches = pattern.findall(content)
            for name, bit in matches:
                bits[name] = int(bit)

            # GC_TYPE_MASK                0x0000000f
            # GC_INFO_SHIFT               10
            pattern = re.compile(r'#define (GC_[^\s]+)\s+((0x)?[0-9a-f]+)')
            matches = pattern.findall(content)
            for name, bit, _ in matches:
                if name == 'GC_TYPE_MASK':
                    self._type_mask = int(bit, 0)
                elif name == 'GC_FLAGS_MASK':
                    self._flags_mask = int(bit, 0)
                elif name == 'GC_INFO_MASK':
                    self._info_mask = int(bit, 0)
                elif name == 'GC_INFO_SHIFT':
                    self._info_shift = int(bit, 0)
                elif name == 'GC_FLAGS_SHIFT':
                    self._flags_shift = int(bit, 0)

            # IS_STR_INTERNED             GC_IMMUTABLE
            # IS_STR_PERMANENT            (1<<8)
            pattern = re.compile(r'#define (IS_(STR|ARRAY|OBJ)_[^\s]+)\s+(\(\s*1\s*<<\s*([0-9]+)\s*\)|GC_[a-zA-Z_]+)')
            matches = pattern.findall(content)
            for name, type, val, bit in matches:
                if bit == '':
                    bit = bits.get(val)
                    if bit == None:
                        continue
                if type == 'STR':
                    target = str_bits
                elif type == 'ARRAY':
                    target = array_bits
                elif type == 'OBJ':
                    target = obj_bits
                target[name] = int(bit)

        # Hard coded because these are not exposed in header files
        self._gc_address = 0x0fffff
        self._gc_color   = 0x300000
        self._gc_black   = 0x000000
        self._gc_white   = 0x100000
        self._gc_grey    = 0x200000
        self._gc_purple  = 0x300000

        self._bits = bits
        self._str_bits = str_bits
        self._array_bits = array_bits
        self._obj_bits = obj_bits

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

def format_nested(value):
    orig_value = value
    type = value.type

    # Null pointers
    if type.code == gdb.TYPE_CODE_PTR and int(value) == 0:
        return orig_value

    addr = orig_value.address

    while type.code == gdb.TYPE_CODE_PTR:
        addr = int(value)
        type = type.target()
        try:
            value = value.dereference()
        except:
            pass

    type = gdb.types.get_basic_type(type)

    if type.tag and re.match(r'^_zend_string$', type.tag):
        return format_zstr(value)
    elif type.tag and re.match(r'^_zend_class_entry$', type.tag):
        return '((zend_class_entry*)0x%x) %s' % (addr, format_zstr(value['name']))
    elif type.tag and re.match(r'^_zend_array$', type.tag):
        return '((zend_array*)0x%x) array(%d)' % (addr, value['nNumOfElements'])

    return orig_value

gdb.printing.register_pretty_printer(gdb, pp_set, replace=True)
