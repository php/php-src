require_relative 'utils'
require 'stringio'

class OpenSSL::TestBuffering < Test::Unit::TestCase

  class IO
    include OpenSSL::Buffering

    attr_accessor :sync

    def initialize
      @io = ""
      def @io.sync
        true
      end

      super

      @sync = false
    end

    def string
      @io
    end

    def sysread(size)
      str = @io.slice!(0, size)
      raise EOFError if str.empty?
      str
    end

    def syswrite(str)
      @io << str
      str.size
    end
  end

  def setup
    @io = IO.new
  end

  def test_flush
    @io.write 'a'

    refute @io.sync
    assert_empty @io.string

    assert_equal @io, @io.flush

    refute @io.sync
    assert_equal 'a', @io.string
  end

  def test_flush_error
    @io.write 'a'

    refute @io.sync
    assert_empty @io.string

    def @io.syswrite *a
      raise SystemCallError, 'fail'
    end

    assert_raises SystemCallError do
      @io.flush
    end

    refute @io.sync, 'sync must not change'
  end

  def test_getc
    @io.syswrite('abc')
    res = []
    assert_equal(?a, @io.getc)
    assert_equal(?b, @io.getc)
    assert_equal(?c, @io.getc)
  end

  def test_each_byte
    @io.syswrite('abc')
    res = []
    @io.each_byte do |c|
      res << c
    end
    assert_equal([97, 98, 99], res)
  end

end if defined?(OpenSSL)
