# $Id$
#
# scanf for Ruby
#
# Some not very comprehensive tests of block behavior.


require 'test/unit'
require 'scanf'
require 'tmpdir'

class TestScanfBlock < Test::Unit::TestCase

  def setup
    @str = <<-EOS
    Beethoven  1770
    Bach       1685
    Handel     1685
    Scarlatti  1685
    Brahms     1833
    EOS
  end

alias set_up setup
  def test_str1
    res = @str.scanf("%s%d") { |name, year| "#{name} was born in #{year}." }
    assert_equal(res,
    [ "Beethoven was born in 1770.",
      "Bach was born in 1685.",
      "Handel was born in 1685.",
      "Scarlatti was born in 1685.",
      "Brahms was born in 1833." ])
  end

  def test_str2
    names = @str.scanf("%s%d") { |name, year| name.upcase }
    assert_equal(names, ["BEETHOVEN", "BACH", "HANDEL", "SCARLATTI", "BRAHMS"])
  end

  def test_str3
    assert_equal("".scanf("%d%f%s") {}, [])
  end

  def test_str4
    assert_equal("abc".scanf("%d%f%s") {}, [])
  end

  def test_str5
    assert_equal("abc".scanf("") {}, [])
  end

  def test_io1
    fn = "#{Dir.tmpdir}/iotest.dat.#{$$}"
    File.open(fn, "w") { |fh| fh.puts(@str) }
    fh = File.open(fn, "rb")
    res = fh.scanf("%s%d") { |name, year| "#{name} was born in #{year}." }

    assert_equal(
    [ "Beethoven was born in 1770.",
      "Bach was born in 1685.",
      "Handel was born in 1685.",
      "Scarlatti was born in 1685.",
      "Brahms was born in 1833." ],res)
    fh.close
  ensure
    File.delete(fn)
  end

  def test_io2
    fn = "#{Dir.tmpdir}/iotest.dat.#{$$}"
    File.open(fn, "w").close
    fh = File.open(fn,"rb")
    assert_equal(fh.scanf("") {}, [])
    fh.seek(0)
    assert_equal(fh.scanf("%d%f%s") {}, [])
    fh.close
  ensure
    File.delete(fn)
  end

end
