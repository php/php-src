
"""GDB support for PHP types

Add this to your gdb by amending your ~/.gdbinit as follows:

    source .gdb.py

Use
  (gdb) p /r any_variable
to print |any_variable| without using any printers.

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
        self.load_bits()

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
                type_name = self.bits.get(bit)
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

    def load_bits(self):
        (symbol,_) = gdb.lookup_symbol("zend_gc_refcount")
        if symbol == None:
            raise "Could not find zend_types.h: symbol zend_gc_refcount not found"
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

        self.bits = bits
        self.types = types
pp_set.add_printer('zend_type', '^zend_type$', ZendTypePrettyPrinter)

gdb.printing.register_pretty_printer(gdb, pp_set, replace=True)
