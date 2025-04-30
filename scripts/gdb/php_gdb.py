
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
  (gdb) set print pretty on

To interactively type Python for development of the printers, use
  (gdb) python foo = gdb.parse_and_eval('bar')
to put the C value 'bar' in the current scope into a Python variable 'foo'.
Then you can interact with that variable:
  (gdb) python print foo['impl_']
"""

import gdb
import re

pp_set = gdb.printing.RegexpCollectionPrettyPrinter("php")

class ZendStringPrettyPrinter(gdb.printing.PrettyPrinter):
    "Print a zend_string"

    def __init__(self, val):
        self.val = val

    def to_string(self):
        return self.format_string()

    def children(self):
        for field in self.val.type.fields():
            if field.name == 'val':
                yield ('val', self.format_string())
            else:
                yield (field.name, self.val[field.name])

    def format_string(self):
        len = int(self.val['len'])
        truncated = False
        if len > 50:
            len = 50
            truncated = True

        ptr_type = gdb.lookup_type('char').pointer()
        ary_type = gdb.lookup_type('char').array(len)
        str = self.val['val'].cast(ptr_type).dereference().cast(ary_type)
        str = str.format_string()
        if truncated:
            str += ' (%d bytes total)' % int(self.val['len'])

        return str


pp_set.add_printer('zend_string', '^_zend_string$', ZendStringPrettyPrinter)

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
                        parts.append(ZendStringPrettyPrinter(str).to_string())
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
                yield (field.name, ZendStringPrettyPrinter(val[field.name].dereference()).to_string())
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
            return ZendStringPrettyPrinter(self.val['value']['str'].dereference()).to_string()
        elif t == type_name_to_bit['array']:
            return 'array'
        elif t == type_name_to_bit['object']:
            return 'object(%s)' % ZendStringPrettyPrinter(self.val['value']['obj']['ce']['name'].dereference()).to_string()
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
                    value = value['ht'].dereference()
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

type_bit_to_name = None
type_name_to_bit = None

def load_type_bits():
    global type_bit_to_name
    global type_name_to_bit

    if type_bit_to_name != None:
        return

    (symbol,_) = gdb.lookup_symbol("zend_gc_refcount")
    if symbol == None:
        raise Exception("Could not find zend_types.h: symbol zend_gc_refcount not found")
    filename = symbol.symtab.fullname()

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

gdb.printing.register_pretty_printer(gdb, pp_set, replace=True)
