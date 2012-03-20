# empty program
assert_equal '',                ''
assert_equal '',                ' '
assert_equal '',                "\n"

# special const
assert_equal 'true',            'true'
assert_equal 'TrueClass',       'true.class'
assert_equal 'false',           'false'
assert_equal 'FalseClass',      'false.class'
assert_equal '',                'nil'
assert_equal 'nil',             'nil.inspect'
assert_equal 'NilClass',        'nil.class'
assert_equal 'sym',             ':sym'
assert_equal ':sym',            ':sym.inspect'
assert_equal 'Symbol',          ':sym.class'
assert_equal '1234',            '1234'
assert_equal 'Fixnum',          '1234.class'
assert_equal '1234',            '1_2_3_4'
assert_equal 'Fixnum',          '1_2_3_4.class'
assert_equal '18',              '0x12'
assert_equal 'Fixnum',          '0x12.class'
assert_equal '15',              '0o17'
assert_equal 'Fixnum',          '0o17.class'
assert_equal '5',               '0b101'
assert_equal 'Fixnum',          '0b101.class'
assert_equal '123456789012345678901234567890', '123456789012345678901234567890'
assert_equal 'Bignum',          '123456789012345678901234567890.class'
assert_equal '2.0',             '2.0'
assert_equal 'Float',           '1.3.class'

# self
assert_equal 'main',            'self'
assert_equal 'Object',          'self.class'

# string literal
assert_equal 'a',               '?a'
assert_equal 'String',          '?a.class'
assert_equal 'A',               '?A'
assert_equal 'String',          '?A.class'
assert_equal "\n",              '?\n'
assert_equal 'String',          '?\n.class'
assert_equal ' ',               '?\ '
assert_equal 'String',          '?\ .class'
assert_equal 'string',          "'string'"
assert_equal 'string',          '"string"'
assert_equal 'string',          '%(string)'
assert_equal 'string',          '%q(string)'
assert_equal 'string',          '%Q(string)'
assert_equal 'string string',   '"string string"'
assert_equal ' ',               '" "'
assert_equal "\0",              '"\0"'
assert_equal "\1",              '"\1"'
assert_equal "3",               '"\x33"'
assert_equal "\n",              '"\n"'

# dynamic string literal
assert_equal '2',               '"#{1 + 1}"'
assert_equal '16',              '"#{2 ** 4}"'
assert_equal 'string',          's = "string"; "#{s}"'

# dynamic symbol literal
assert_equal 'a3c',             ':"a#{1+2}c"'
assert_equal ':a3c',            ':"a#{1+2}c".inspect'
assert_equal 'Symbol',          ':"a#{1+2}c".class'

# xstring
assert_equal "foo\n",           %q(`echo foo`)
assert_equal "foo\n",           %q(s = "foo"; `echo #{s}`)

# regexp
assert_equal '',                '//.source'
assert_equal 'Regexp',          '//.class'
assert_equal '0',               '// =~ "a"'
assert_equal '0',               '// =~ ""'
assert_equal 'a',               '/a/.source'
assert_equal 'Regexp',          '/a/.class'
assert_equal '0',               '/a/ =~ "a"'
assert_equal '0',               '/test/ =~ "test"'
assert_equal '',                '/test/ =~ "tes"'
assert_equal '0',               're = /test/; re =~ "test"'
assert_equal '0',               'str = "test"; /test/ =~ str'
assert_equal '0',               're = /test/; str = "test"; re =~ str'

# dynacmi regexp
assert_equal 'regexp',          %q(/re#{'ge'}xp/.source)
assert_equal 'Regexp',          %q(/re#{'ge'}xp/.class)

# array
assert_equal 'Array',           '[].class'
assert_equal '0',               '[].size'
assert_equal '0',               '[].length'
assert_equal '[]',              '[].inspect'
assert_equal 'Array',           '[0].class'
assert_equal '1',               '[3].size'
assert_equal '[3]',             '[3].inspect'
assert_equal '3',               'a = [3]; a[0]'
assert_equal 'Array',           '[1,2].class'
assert_equal '2',               '[1,2].size'
assert_equal '[1, 2]',           '[1,2].inspect'
assert_equal 'Array',           '[1,2,3,4,5].class'
assert_equal '5',               '[1,2,3,4,5].size'
assert_equal '[1, 2, 3, 4, 5]', '[1,2,3,4,5].inspect'
assert_equal '1',               'a = [1,2]; a[0]'
assert_equal '2',               'a = [1,2]; a[1]'
assert_equal 'Array',           'a = [1 + 2, 3 + 4, 5 + 6]; a.class'
assert_equal '[3, 7, 11]',      'a = [1 + 2, 3 + 4, 5 + 6]; a.inspect'
assert_equal '7',               'a = [1 + 2, 3 + 4, 5 + 6]; a[1]'
assert_equal '1',               '([0][0] += 1)'
assert_equal '1',               '([2][0] -= 1)'
assert_equal 'Array',           'a = [obj = Object.new]; a.class'
assert_equal '1',               'a = [obj = Object.new]; a.size'
assert_equal 'true',            'a = [obj = Object.new]; a[0] == obj'
assert_equal '5',               'a = [1,2,3]; a[1] = 5; a[1]'
assert_equal 'bar',             '[*:foo];:bar'
assert_equal '[1, 2]',          'def nil.to_a; [2]; end; [1, *nil]'
assert_equal '[1, 2]',          'def nil.to_a; [1, 2]; end; [*nil]'
assert_equal '[0, 1, {2=>3}]',  '[0, *[1], 2=>3]', "[ruby-dev:31592]"


# hash
assert_equal 'Hash',            '{}.class'
assert_equal '{}',              '{}.inspect'
assert_equal 'Hash',            '{1=>2}.class'
assert_equal '{1=>2}',          '{1=>2}.inspect'
assert_equal '2',               'h = {1 => 2}; h[1]'
assert_equal '0',               'h = {1 => 2}; h.delete(1); h.size'
assert_equal '',                'h = {1 => 2}; h.delete(1); h[1]'
assert_equal '2',               'h = {"string" => "literal", "goto" => "hell"}; h.size'
assert_equal 'literal', 'h = {"string"=>"literal", "goto"=>"hell"}; h["string"]'
assert_equal 'hell', 'h = {"string"=>"literal", "goto"=>"hell"}; h["goto"]'

# range
assert_equal 'Range',           '(1..2).class'
assert_equal '1..2',            '(1..2).inspect'
assert_equal '1',               '(1..2).begin'
assert_equal '2',               '(1..2).end'
assert_equal 'false',           '(1..2).exclude_end?'
assert_equal 'Range',           'r = 1..2; r.class'
assert_equal '1..2',            'r = 1..2; r.inspect'
assert_equal '1',               'r = 1..2; r.begin'
assert_equal '2',               'r = 1..2; r.end'
assert_equal 'false',           'r = 1..2; r.exclude_end?'
assert_equal 'Range',           '(1...3).class'
assert_equal '1...3',           '(1...3).inspect'
assert_equal '1',               '(1...3).begin'
assert_equal '3',               '(1...3).end'
assert_equal 'true',            '(1...3).exclude_end?'
assert_equal 'Range',           'r = (1...3); r.class'
assert_equal '1...3',           'r = (1...3); r.inspect'
assert_equal '1',               'r = (1...3); r.begin'
assert_equal '3',               'r = (1...3); r.end'
assert_equal 'true',            'r = (1...3); r.exclude_end?'
assert_equal 'Range',           'r = (1+2 .. 3+4); r.class'
assert_equal '3..7',            'r = (1+2 .. 3+4); r.inspect'
assert_equal '3',               'r = (1+2 .. 3+4); r.begin'
assert_equal '7',               'r = (1+2 .. 3+4); r.end'
assert_equal 'false',           'r = (1+2 .. 3+4); r.exclude_end?'
assert_equal 'Range',           'r = (1+2 ... 3+4); r.class'
assert_equal '3...7',           'r = (1+2 ... 3+4); r.inspect'
assert_equal '3',               'r = (1+2 ... 3+4); r.begin'
assert_equal '7',               'r = (1+2 ... 3+4); r.end'
assert_equal 'true',            'r = (1+2 ... 3+4); r.exclude_end?'
assert_equal 'Range',           'r = ("a".."c"); r.class'
assert_equal '"a".."c"',        'r = ("a".."c"); r.inspect'
assert_equal 'a',               'r = ("a".."c"); r.begin'
assert_equal 'c',               'r = ("a".."c"); r.end'

assert_equal 'String',          '__FILE__.class'
assert_equal 'Fixnum',          '__LINE__.class'

###

assert_equal 'ok', %q{
  # this cause "called on terminated object".
  ObjectSpace.each_object(Module) {|m| m.name.inspect }
  :ok
}

assert_normal_exit %q{
  begin
    r = 0**-1
    r + r
  rescue
  end
}, '[ruby-dev:34524]'

assert_normal_exit %q{
  begin
    r = Marshal.load("\x04\bU:\rRational[\ai\x06i\x05")
    r + r
  rescue
  end
}, '[ruby-dev:34536]'

assert_equal 'ok', %q{
  "#{}""#{}ok"
}, '[ruby-dev:38968]'

assert_equal 'ok', %q{
  "#{}o""#{}k""#{}"
}, '[ruby-core:25284]'
