require 'test/unit'

class TestRubyLiteral < Test::Unit::TestCase

  def test_special_const
    assert_equal 'true', true.inspect
    assert_instance_of TrueClass, true
    assert_equal 'false', false.inspect
    assert_instance_of FalseClass, false
    assert_equal 'nil', nil.inspect
    assert_instance_of NilClass, nil
    assert_equal ':sym', :sym.inspect
    assert_instance_of Symbol, :sym
    assert_equal '1234', 1234.inspect
    assert_instance_of Fixnum, 1234
    assert_equal '1234', 1_2_3_4.inspect
    assert_instance_of Fixnum, 1_2_3_4
    assert_equal '18', 0x12.inspect
    assert_instance_of Fixnum, 0x12
    assert_raise(SyntaxError) { eval("0x") }
    assert_equal '15', 0o17.inspect
    assert_instance_of Fixnum, 0o17
    assert_raise(SyntaxError) { eval("0o") }
    assert_equal '5', 0b101.inspect
    assert_instance_of Fixnum, 0b101
    assert_raise(SyntaxError) { eval("0b") }
    assert_equal '123456789012345678901234567890', 123456789012345678901234567890.inspect
    assert_instance_of Bignum, 123456789012345678901234567890
    assert_instance_of Float, 1.3
    assert_equal '2', eval("0x00+2").inspect
  end

  def test_self
    assert_equal self, self
    assert_instance_of TestRubyLiteral, self
    assert_respond_to self, :test_self
  end

  def test_string
    assert_instance_of String, ?a
    assert_equal "a", ?a
    assert_instance_of String, ?A
    assert_equal "A", ?A
    assert_instance_of String, ?\n
    assert_equal "\n", ?\n
    assert_equal " ", ?\   # space
    assert_equal '', ''
    assert_equal 'string', 'string'
    assert_equal 'string string', 'string string'
    assert_equal ' ', ' '
    assert_equal ' ', " "
    assert_equal "\0", "\0"
    assert_equal "\1", "\1"
    assert_equal "3", "\x33"
    assert_equal "\n", "\n"
    bug2500 = '[ruby-core:27228]'
    bug5262 = '[ruby-core:39222]'
    %w[c C- M-].each do |pre|
      ["u", %w[u{ }]].each do |open, close|
        ["?", ['"', '"']].each do |qopen, qclose|
          str = "#{qopen}\\#{pre}\\#{open}5555#{close}#{qclose}"
          assert_raise(SyntaxError, "#{bug2500} eval(#{str})") {eval(str)}

          str = "#{qopen}\\#{pre}\\#{open}\u201c#{close}#{qclose}"
          assert_raise(SyntaxError, "#{bug5262} eval(#{str})") {eval(str)}

          str = "#{qopen}\\#{pre}\\#{open}\u201c#{close}#{qclose}".encode("euc-jp")
          assert_raise(SyntaxError, "#{bug5262} eval(#{str})") {eval(str)}

          str = "#{qopen}\\#{pre}\\#{open}\u201c#{close}#{qclose}".encode("iso-8859-13")
          assert_raise(SyntaxError, "#{bug5262} eval(#{str})") {eval(str)}

          str = "#{qopen}\\#{pre}\\#{open}\xe2\x7f#{close}#{qclose}".force_encoding("utf-8")
          assert_raise(SyntaxError, "#{bug5262} eval(#{str})") {eval(str)}
        end
      end
    end
    bug6069 = '[ruby-dev:45278]'
    assert_equal "\x13", "\c\x33"
    assert_equal "\x13", "\C-\x33"
    assert_equal "\xB3", "\M-\x33"
    assert_equal "\u201c", eval(%["\\\u{201c}"]), bug5262
    assert_equal "\u201c".encode("euc-jp"), eval(%["\\\u{201c}"].encode("euc-jp")), bug5262
    assert_equal "\u201c".encode("iso-8859-13"), eval(%["\\\u{201c}"].encode("iso-8859-13")), bug5262
    assert_equal "\\\u201c", eval(%['\\\u{201c}']), bug6069
    assert_equal "\\\u201c".encode("euc-jp"), eval(%['\\\u{201c}'].encode("euc-jp")), bug6069
    assert_equal "\\\u201c".encode("iso-8859-13"), eval(%['\\\u{201c}'].encode("iso-8859-13")), bug6069
    assert_equal "\u201c", eval(%[?\\\u{201c}]), bug6069
    assert_equal "\u201c".encode("euc-jp"), eval(%[?\\\u{201c}].encode("euc-jp")), bug6069
    assert_equal "\u201c".encode("iso-8859-13"), eval(%[?\\\u{201c}].encode("iso-8859-13")), bug6069
  end

  def test_dstring
    assert_equal '2', "#{1+1}"
    assert_equal '16', "#{2 ** 4}"
    s = "string"
    assert_equal s, "#{s}"
    a = 'Foo'
    b = "#{a}" << 'Bar'
    assert_equal('Foo', a, 'r3842')
    assert_equal('FooBar', b, 'r3842')
  end

  def test_dsymbol
    assert_equal :a3c, :"a#{1+2}c"
  end

  def test_xstring
    assert_equal "foo\n", `echo foo`
    s = 'foo'
    assert_equal "foo\n", `echo #{s}`
  end

  def test_regexp
    assert_instance_of Regexp, //
    assert_match(//, 'a')
    assert_match(//, '')
    assert_instance_of Regexp, /a/
    assert_match(/a/, 'a')
    assert_no_match(/test/, 'tes')
    re = /test/
    assert_match re, 'test'
    str = 'test'
    assert_match re, str
    assert_match(/test/, str)
    assert_equal 0, (/test/ =~ 'test')
    assert_equal 0, (re =~ 'test')
    assert_equal 0, (/test/ =~ str)
    assert_equal 0, (re =~ str)
    assert_equal 0, ('test' =~ /test/)
    assert_equal 0, ('test' =~ re)
    assert_equal 0, (str =~ /test/)
    assert_equal 0, (str =~ re)
  end

  def test_dregexp
    assert_instance_of Regexp, /re#{'ge'}xp/
    assert_equal(/regexp/, /re#{'ge'}xp/)
    bug3903 = '[ruby-core:32682]'
    assert_raise(SyntaxError, bug3903) {eval('/[#{"\x80"}]/')}
  end

  def test_array
    assert_instance_of Array, []
    assert_equal [], []
    assert_equal 0, [].size
    assert_instance_of Array, [0]
    assert_equal [3], [3]
    assert_equal 1, [3].size
    a = [3]
    assert_equal 3, a[0]
    assert_instance_of Array, [1,2]
    assert_equal [1,2], [1,2]
    assert_instance_of Array, [1,2,3,4,5]
    assert_equal [1,2,3,4,5], [1,2,3,4,5]
    assert_equal 5, [1,2,3,4,5].size
    a = [1,2]
    assert_equal 1, a[0]
    assert_equal 2, a[1]
    a = [1 + 2, 3 + 4, 5 + 6]
    assert_instance_of Array, a
    assert_equal [3, 7, 11], a
    assert_equal 7, a[1]
    assert_equal 1, ([0][0] += 1)
    assert_equal 1, ([2][0] -= 1)
    a = [obj = Object.new]
    assert_instance_of Array, a
    assert_equal 1, a.size
    assert_equal obj, a[0]
    a = [1,2,3]
    a[1] = 5
    assert_equal 5, a[1]
  end

  def test_hash
    assert_instance_of Hash, {}
    assert_equal({}, {})
    assert_instance_of Hash, {1 => 2}
    assert_equal({1 => 2}, {1 => 2})
    h = {1 => 2}
    assert_equal 2, h[1]
    h = {"string" => "literal", "goto" => "hell"}
    assert_equal h, h
    assert_equal 2, h.size
    assert_equal h, h
    assert_equal "literal", h["string"]
  end

  def test_range
    assert_instance_of Range, (1..2)
    assert_equal(1..2, 1..2)
    r = 1..2
    assert_equal 1, r.begin
    assert_equal 2, r.end
    assert_equal false, r.exclude_end?
    assert_instance_of Range, (1...3)
    assert_equal(1...3, 1...3)
    r = 1...3
    assert_equal 1, r.begin
    assert_equal 3, r.end
    assert_equal true, r.exclude_end?
    r = 1+2 .. 3+4
    assert_instance_of Range, r
    assert_equal 3, r.begin
    assert_equal 7, r.end
    assert_equal false, r.exclude_end?
    r = 1+2 ... 3+4
    assert_instance_of Range, r
    assert_equal 3, r.begin
    assert_equal 7, r.end
    assert_equal true, r.exclude_end?
    assert_instance_of Range, 'a'..'c'
    r = 'a'..'c'
    assert_equal 'a', r.begin
    assert_equal 'c', r.end
  end

  def test__FILE__
    assert_instance_of String, __FILE__
    assert_equal __FILE__, __FILE__
    assert_equal 'test_literal.rb', File.basename(__FILE__)
  end

  def test__LINE__
    assert_instance_of Fixnum, __LINE__
    assert_equal __LINE__, __LINE__
  end

  def test_integer
    head = ['', '0x', '0o', '0b', '0d', '-', '+']
    chars = ['0', '1', '_', '9', 'f']
    head.each {|h|
      4.times {|len|
        a = [h]
        len.times { a = a.product(chars).map {|x| x.join('') } }
        a.each {|s|
          next if s.empty?
          begin
            r1 = Integer(s)
          rescue ArgumentError
            r1 = :err
          end
          begin
            r2 = eval(s)
          rescue NameError, SyntaxError
            r2 = :err
          end
          assert_equal(r1, r2, "Integer(#{s.inspect}) != eval(#{s.inspect})")
        }
      }
    }
    bug2407 = '[ruby-dev:39798]'
    head.each {|h|
      if /^0/ =~ h
        begin
          eval("#{h}_")
        rescue SyntaxError => e
          assert_match(/numeric literal without digits\Z/, e.message, bug2407)
        end
      end
    }
  end

  def test_float
    head = ['', '-', '+']
    chars = ['0', '1', '_', '9', 'f', '.']
    head.each {|h|
      6.times {|len|
        a = [h]
        len.times { a = a.product(chars).map {|x| x.join('') } }
        a.each {|s|
          next if s.empty?
          next if /\.\z/ =~ s
          next if /\A[-+]?\./ =~ s
          next if /\A[-+]?0/ =~ s
          begin
            r1 = Float(s)
          rescue ArgumentError
            r1 = :err
          end
          begin
            r2 = eval(s)
          rescue NameError, SyntaxError
            r2 = :err
          end
          r2 = :err if Range === r2
          assert_equal(r1, r2, "Float(#{s.inspect}) != eval(#{s.inspect})")
        }
      }
    }
  end

end
