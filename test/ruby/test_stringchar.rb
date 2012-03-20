require 'test/unit'

class TestStringchar < Test::Unit::TestCase
  def test_string
    assert_equal("abcd", "abcd")
    assert_match(/abcd/, "abcd")
    assert("abcd" === "abcd")
    # compile time string concatenation
    assert_equal("abcd", "ab" "cd")
    assert_equal("22aacd44", "#{22}aa" "cd#{44}")
    assert_equal("22aacd445566", "#{22}aa" "cd#{44}" "55" "#{66}")
    assert("abc" !~ /^$/)
    assert("abc\n" !~ /^$/)
    assert("abc" !~ /^d*$/)
    assert_equal(3, ("abc" =~ /d*$/))
    assert("" =~ /^$/)
    assert("\n" =~ /^$/)
    assert("a\n\n" =~ /^$/)
    assert("abcabc" =~ /.*a/); assert_equal("abca", $&)
    assert("abcabc" =~ /.*c/); assert_equal("abcabc", $&)
    assert("abcabc" =~ /.*?a/); assert_equal("a", $&)
    assert("abcabc" =~ /.*?c/); assert_equal("abc", $&)
    assert(/(.|\n)*?\n(b|\n)/ =~ "a\nb\n\n"); assert_equal("a\nb", $&)

    assert(/^(ab+)+b/ =~ "ababb"); assert_equal("ababb", $&)
    assert(/^(?:ab+)+b/ =~ "ababb"); assert_equal("ababb", $&)
    assert(/^(ab+)+/ =~ "ababb"); assert_equal("ababb", $&)
    assert(/^(?:ab+)+/ =~ "ababb"); assert_equal("ababb", $&)

    assert(/(\s+\d+){2}/ =~ " 1 2"); assert_equal(" 1 2", $&)
    assert(/(?:\s+\d+){2}/ =~ " 1 2"); assert_equal(" 1 2", $&)

    $x = <<END;
ABCD
ABCD
END
    $x.gsub!(/((.|\n)*?)B((.|\n)*?)D/m ,'\1\3')
    assert_equal("AC\nAC\n", $x)

    assert_match(/foo(?=(bar)|(baz))/, "foobar")
    assert_match(/foo(?=(bar)|(baz))/, "foobaz")

    $foo = "abc"
    assert_equal("abc = abc", "#$foo = abc")
    assert_equal("abc = abc", "#{$foo} = abc")

    foo = "abc"
    assert_equal("abc = abc", "#{foo} = abc")

    assert_equal('-----', '-' * 5)
    assert_equal('-', '-' * 1)
    assert_equal('', '-' * 0)

    foo = '-'
    assert_equal('-----', foo * 5)
    assert_equal('-', foo * 1)
    assert_equal('', foo * 0)

    $x = "a.gif"
    assert_equal("gif", $x.sub(/.*\.([^\.]+)$/, '\1'))
    assert_equal("b.gif", $x.sub(/.*\.([^\.]+)$/, 'b.\1'))
    assert_equal("", $x.sub(/.*\.([^\.]+)$/, '\2'))
    assert_equal("ab", $x.sub(/.*\.([^\.]+)$/, 'a\2b'))
    assert_equal("<a.gif>", $x.sub(/.*\.([^\.]+)$/, '<\&>'))
  end

  def test_char
    # character constants(assumes ASCII)
    assert_equal(?a, "a"[0])
    assert_equal(?a, ?a)
    assert_equal("\1", ?\C-a)
    assert_equal("\341", ?\M-a)
    assert_equal("\201", ?\M-\C-a)
    assert_equal(?A, "a".upcase![0])
    assert_equal(?a, "A".downcase![0])
    assert_equal("ABC", "abc".tr!("a-z", "A-Z"))
    assert_equal("ABC", "aabbcccc".tr_s!("a-z", "A-Z"))
    assert_equal("abc", "abcc".squeeze!("a-z"))
    assert_equal("ad", "abcd".delete!("bc"))

    $x = "abcdef"
    $y = [ ?a, ?b, ?c, ?d, ?e, ?f ]
    $bad = false
    $x.each_byte {|i|
      if i.chr != $y.shift
        $bad = true
        break
      end
    }
    assert(!$bad)

    s = "a string"
    s[0..s.size]="another string"
    assert_equal("another string", s)

    s = <<EOS
#{
[1,2,3].join(",")
}
EOS
    assert_equal("1,2,3\n", s)
    assert_equal(926381, "Just".to_i(36))
    assert_equal(-23200231779, "-another".to_i(36))
    assert_equal("ruby", 1299022.to_s(36))
    assert_equal("-hacker", -1045307475.to_s(36))
    assert_equal(265419172580680477752431643787347, "Just_another_Ruby_hacker".to_i(36))
    assert_equal("-justanotherrubyhacker", -265419172580680477752431643787347.to_s(36))

    a = []
    (0..255).each {|n|
      ch = [n].pack("C")
      a.push ch if /a#{Regexp.quote ch}b/x =~ "ab"
    }
    assert_equal(0, a.size)
  end

  def test_bang
    s = "aBc"
    s.upcase
    assert_equal("aBc", s)
    s.upcase!
    assert_equal("ABC", s)

    s = "aBc"
    s.downcase
    assert_equal("aBc", s)
    s.downcase!
    assert_equal("abc", s)

    s = "aBc"
    s.swapcase
    assert_equal("aBc", s)
    s.swapcase!
    assert_equal("AbC", s)

    s = "aBc"
    s.capitalize
    assert_equal("aBc", s)
    s.capitalize!
    assert_equal("Abc", s)

    s = "aBc"
    s.tr("a-z", "A-Z")
    assert_equal("aBc", s)
    s.tr!("a-z", "A-Z")
    assert_equal("ABC", s)

    s = "aaBBcc"
    s.tr_s("a-z", "A-Z")
    assert_equal("aaBBcc", s)
    s.tr_s!("a-z", "A-Z")
    assert_equal("ABBC", s)

    s = "aaBBcc"
    s.squeeze("a-z")
    assert_equal("aaBBcc", s)
    s.squeeze!("a-z")
    assert_equal("aBBc", s)

    s = "aaBBcc"
    s.delete("a-z")
    assert_equal("aaBBcc", s)
    s.delete!("a-z")
    assert_equal("BB", s)
  end

  def test_dump
    bug3996 = '[ruby-core:32935]'
    Encoding.list.find_all {|enc| enc.ascii_compatible?}.each do |enc|
      (0..256).map do |c|
        begin
          s = c.chr(enc)
        rescue RangeError, ArgumentError
          break
        else
          assert_not_match(/\0/, s.dump, bug3996)
        end
      end
    end
  end
end
