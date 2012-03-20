require "test/unit"

class TestGB18030 < Test::Unit::TestCase
  def s(s)
    s.force_encoding("gb18030")
  end

  def test_mbc_enc_len
    assert_equal(1, s("\x81\x40").size)
    assert_equal(1, s("\x81\x30\x81\x30").size)
  end

  def test_mbc_to_code
    assert_equal(0x8140, s("\x81\x40").ord)
  end

  def test_code_to_mbc
    assert_equal(s("\x81\x40"), 0x8140.chr("gb18030"))
  end

  def test_mbc_case_fold
    r = Regexp.new(s("(\x81\x40)\\1"), "i")
    assert_match(r, s("\x81\x40\x81\x40"))
  end

  def scheck(c, i)
    assert_equal(s(c.reverse.take(c.size - i).join), s(c.reverse.join).chop)
  end

  def fcheck(c)
    c = s(c.reverse.join)
    assert_raise(ArgumentError, c) { c.chop }
  end

  def test_left_adjust_char_head
    # C1: 00-2f, 3a-3f, 7f, ff
    # C2: 40-7e, 80
    # C4: 30-39
    # CM: 81-fe
    c1 = "\x2f"
    c2 = "\x40"
    c4 = "\x30"
    cm = "\x81"

    # S_START-c1
    # S_START-c2-S_one_C2-0
    # S_START-c2-S_one_C2-c1
    # S_START-c2-S_one_C2-cm-S_odd_CM_one_CX-c1
    # S_START-c2-S_one_C2-cm-S_odd_CM_one_CX-cm-S_even_CM_one_CX-c1
    # S_START-c2-S_one_C2-cm-S_odd_CM_one_CX-cm-S_even_CM_one_CX-cm-S_odd_CM_one_CX(rec)
    # S_START-c4-S_one_C4-c1
    # S_START-c4-S_one_C4-cm-S_one_CMC4-c1
    # S_START-c4-S_one_C4-cm-S_one_CMC4-c4-S_one_C4_odd_CMC4-c1
    # S_START-c4-S_one_C4-cm-S_one_CMC4-c4-S_one_C4_odd_CMC4-cm-S_even_CMC4-c1
    # S_START-c4-S_one_C4-cm-S_one_CMC4-c4-S_one_C4_odd_CMC4-cm-S_even_CMC4-c4-S_one_C4_even_CMC4-c1
    # S_START-c4-S_one_C4-cm-S_one_CMC4-c4-S_one_C4_odd_CMC4-cm-S_even_CMC4-c4-S_one_C4_even_CMC4-cm-S_odd_CMC4-c1
    # S_START-c4-S_one_C4-cm-S_one_CMC4-c4-S_one_C4_odd_CMC4-cm-S_even_CMC4-c4-S_one_C4_even_CMC4-cm-S_odd_CMC4-c4-S_one_C4_odd_CMC4(rec)
    # S_START-c4-S_one_C4-cm-S_one_CMC4-c4-S_one_C4_odd_CMC4-cm-S_even_CMC4-c4-S_one_C4_even_CMC4-cm-S_odd_CMC4-cm-S_odd_CM_odd_CMC4-c1
    # S_START-c4-S_one_C4-cm-S_one_CMC4-c4-S_one_C4_odd_CMC4-cm-S_even_CMC4-c4-S_one_C4_even_CMC4-cm-S_odd_CMC4-cm-S_odd_CM_odd_CMC4-cm-S_even_CM_odd_CMC4-c1
    # S_START-c4-S_one_C4-cm-S_one_CMC4-c4-S_one_C4_odd_CMC4-cm-S_even_CMC4-c4-S_one_C4_even_CMC4-cm-S_odd_CMC4-cm-S_odd_CM_odd_CMC4-cm-S_even_CM_odd_CMC4-cm-S_odd_CM_odd_CMC4(rec)
    # S_START-c4-S_one_C4-cm-S_one_CMC4-c4-S_one_C4_odd_CMC4-cm-S_even_CMC4-cm-S_odd_CM_even_CMC4-c1
    # S_START-c4-S_one_C4-cm-S_one_CMC4-c4-S_one_C4_odd_CMC4-cm-S_even_CMC4-cm-S_odd_CM_even_CMC4-cm-S_even_CM_even_CMC4-c1
    # S_START-c4-S_one_C4-cm-S_one_CMC4-c4-S_one_C4_odd_CMC4-cm-S_even_CMC4-cm-S_odd_CM_even_CMC4-cm-S_even_CM_even_CMC4-cm-S_odd_CM_even_CMC4(rec)
    # S_START-c4-S_one_C4-cm-S_one_CMC4-cm-S_even_CM_one_CX(rec)
    # S_START-cm-S_one_CM-c1
    # S_START-cm-S_one_CM-c4-S_odd_C4CM-c1
    # S_START-cm-S_one_CM-c4-S_odd_C4CM-cm-S_one_CM_odd_C4CM-c1
    # S_START-cm-S_one_CM-c4-S_odd_C4CM-cm-S_one_CM_odd_C4CM-c4-S_even_C4CM-c1
    # S_START-cm-S_one_CM-c4-S_odd_C4CM-cm-S_one_CM_odd_C4CM-c4-S_even_C4CM-cm-S_one_CM_even_C4CM-c1
    # S_START-cm-S_one_CM-c4-S_odd_C4CM-cm-S_one_CM_odd_C4CM-c4-S_even_C4CM-cm-S_one_CM_even_C4CM-c4-S_odd_C4CM(rec)
    # S_START-cm-S_one_CM-c4-S_odd_C4CM-cm-S_one_CM_odd_C4CM-c4-S_even_C4CM-cm-S_one_CM_even_C4CM-cm-S_even_CM_even_C4CM-c1
    # S_START-cm-S_one_CM-c4-S_odd_C4CM-cm-S_one_CM_odd_C4CM-c4-S_even_C4CM-cm-S_one_CM_even_C4CM-cm-S_even_CM_even_C4CM-cm-S_odd_CM_even_C4CM-c1
    # S_START-cm-S_one_CM-c4-S_odd_C4CM-cm-S_one_CM_odd_C4CM-c4-S_even_C4CM-cm-S_one_CM_even_C4CM-cm-S_even_CM_even_C4CM-cm-S_odd_CM_even_C4CM-cm-S_even_CM_even_C4CM(rec)
    # S_START-cm-S_one_CM-c4-S_odd_C4CM-cm-S_one_CM_odd_C4CM-cm-S_even_CM_odd_C4CM-c1
    # S_START-cm-S_one_CM-c4-S_odd_C4CM-cm-S_one_CM_odd_C4CM-cm-S_even_CM_odd_C4CM-cm-S_odd_CM_odd_C4CM-c1
    # S_START-cm-S_one_CM-c4-S_odd_C4CM-cm-S_one_CM_odd_C4CM-cm-S_even_CM_odd_C4CM-cm-S_odd_CM_odd_C4CM-cm-S_even_CM_odd_C4CM(rec)
    # S_START-cm-S_one_CM-cm-S_odd_CM_one_CX(rec)

    scheck([c1], 1)
    scheck([c2], 1)
    scheck([c2, c1], 1)
    scheck([c2, cm, c1], 2)
    scheck([c2, cm, cm, c1], 1)
    scheck([c2, cm, cm, cm], 2)
    scheck([c4], 1)
    scheck([c4, c1], 1)
    scheck([c4, cm], 2)
    scheck([c4, cm, c1], 2)
    scheck([c4, cm, c4, c1], 2)
    scheck([c4, cm, c4, cm], 4)
    scheck([c4, cm, c4, cm, c1], 4)
    scheck([c4, cm, c4, cm, c4], 4)
    scheck([c4, cm, c4, cm, c4, c1], 4)
    scheck([c4, cm, c4, cm, c4, cm], 2)
    scheck([c4, cm, c4, cm, c4, cm, c1], 2)
    scheck([c4, cm, c4, cm, c4, cm, c4], 2)
    scheck([c4, cm, c4, cm, c4, cm, cm, c1], 4)
    scheck([c4, cm, c4, cm, c4, cm, cm, cm], 2)
    scheck([c4, cm, c4, cm, c4, cm, cm, cm, c1], 2)
    scheck([c4, cm, c4, cm, c4, cm, cm, cm, cm], 4)
    scheck([c4, cm, c4, cm, cm, c1], 2)
    scheck([c4, cm, c4, cm, cm, cm], 4)
    scheck([c4, cm, c4, cm, cm, cm, c1], 4)
    scheck([c4, cm, c4, cm, cm, cm, cm], 2)
    scheck([c4, cm, cm], 1)
    scheck([cm], 1)
    scheck([cm, c1], 1)
    scheck([cm, c4, c1], 1)
    scheck([cm, c4, cm], 3)
    scheck([cm, c4, cm, c1], 3)
    scheck([cm, c4, cm, c4], 3)
    scheck([cm, c4, cm, c4, c1], 3)
    scheck([cm, c4, cm, c4, cm], 1)
    scheck([cm, c4, cm, c4, cm, c1], 1)
    scheck([cm, c4, cm, c4, cm, c4], 1)
    scheck([cm, c4, cm, c4, cm, cm, c1], 3)
    scheck([cm, c4, cm, c4, cm, cm, cm], 1)
    scheck([cm, c4, cm, c4, cm, cm, cm, c1], 1)
    scheck([cm, c4, cm, c4, cm, cm, cm, cm], 3)
    scheck([cm, c4, cm, cm, c1], 1)
    scheck([cm, c4, cm, cm, cm], 3)
    scheck([cm, c4, cm, cm, cm, c1], 3)
    scheck([cm, c4, cm, cm, cm, cm], 1)
    scheck([cm, cm], 2)
  end
end
