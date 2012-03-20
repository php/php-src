require 'test/unit'

require '../tool/jisx0208'

class Test_JISX0208_Char < Test::Unit::TestCase
  def test_create_with_row_cell
    assert_equal JISX0208::Char.new(0x2121), JISX0208::Char.new(1, 1)
  end

  def test_succ
    assert_equal JISX0208::Char.new(0x2221), JISX0208::Char.new(0x217e).succ
    assert_equal JISX0208::Char.new(2, 1), JISX0208::Char.new(1, 94).succ
    assert_equal JISX0208::Char.new(0x7f21), JISX0208::Char.new(0x7e7e).succ
  end

  def test_to_shift_jis
    assert_equal 0x895C, JISX0208::Char.new(0x313D).to_sjis
    assert_equal 0x895C, JISX0208::Char.from_sjis(0x895C).to_sjis
    assert_equal 0xF3DE, JISX0208::Char.from_sjis(0xF3DE).to_sjis
    assert_equal 0xFC40, JISX0208::Char.from_sjis(0xFC40).to_sjis
  end

  def test_from_sjis
    assert_raise(ArgumentError) { JISX0208::Char.from_sjis(-1) }
    assert_raise(ArgumentError) { JISX0208::Char.from_sjis(0x10000) }
    assert_nothing_raised { JISX0208::Char.from_sjis(0x8140) }
    assert_nothing_raised { JISX0208::Char.from_sjis(0xFCFC) }
    assert_equal JISX0208::Char.new(0x313D), JISX0208::Char.from_sjis(0x895C)
  end

  def test_row
    assert_equal 1, JISX0208::Char.new(0x2121).row
    assert_equal 94, JISX0208::Char.new(0x7E7E).row
  end

  def test_cell
    assert_equal 1, JISX0208::Char.new(0x2121).cell
    assert_equal 94, JISX0208::Char.new(0x7E7E).cell
  end
end
