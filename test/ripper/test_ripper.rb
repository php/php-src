begin
  require 'ripper'
  require 'test/unit'
  ripper_test = true
  module TestRipper; end
rescue LoadError
end

class TestRipper::Ripper < Test::Unit::TestCase

  def setup
    @ripper = Ripper.new '1 + 1'
  end

  def test_column
    assert_nil @ripper.column
  end

  def test_encoding
    assert_equal Encoding::US_ASCII, @ripper.encoding
  end

  def test_end_seen_eh
    refute @ripper.end_seen?
  end

  def test_filename
    assert_equal '(ripper)', @ripper.filename
  end

  def test_lineno
    assert_nil @ripper.lineno
  end

  def test_parse
    refute @ripper.parse
  end

  def test_yydebug
    refute @ripper.yydebug
  end

  def test_yydebug_equals
    @ripper.yydebug = true

    assert @ripper.yydebug
  end

end if ripper_test
