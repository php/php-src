require 'test/unit'
require 'digest'
require_relative '../with_different_ofs.rb'

class TestDigestExtend < Test::Unit::TestCase
  extend DifferentOFS

  class MyDigest < Digest::Class
    def initialize(*arg)
      super
      @buf = []
    end

    def initialize_copy(org)
      @buf = org.buf.dup
    end

    def update(arg)
      @buf << arg
      self
    end

    alias << update

    def finish
      (@buf.join('').length % 256).chr
    end

    def reset
      @buf.clear
      self
    end

  protected

    def buf
      @buf
    end
  end

  def setup
    @MyDigest = Class.new(MyDigest)
  end

  def test_digest_s_hexencode
    assert_equal('', Digest.hexencode(''))
    assert_equal('0102', Digest.hexencode("\1\2"))
    assert_equal(
      (0..0xff).to_a.map { |c| sprintf("%02x", c ) }.join(''),
      Digest.hexencode((0..0xff).to_a.map { |c| c.chr }.join(''))
    )
  end

  def test_class_reset
    a = Digest::SHA1.new
    base = a.to_s
    assert_equal(base, a.reset.to_s)
    b = a.new
    assert_equal(base, b.to_s)
    b.update('1')
    assert_not_equal(base, b.to_s)
    assert_equal(base, b.reset.to_s)
  end

  def test_digest
    assert_equal("\3", MyDigest.digest("foo"))
  end

  def test_hexdigest
    assert_equal("03", @MyDigest.hexdigest("foo"))
  end

  def test_context
    digester = @MyDigest.new
    digester.update("foo")
    assert_equal("\3", digester.digest)
    digester.update("foobar")
    assert_equal("\t", digester.digest)
    digester.update("foo")
    assert_equal("\f", digester.digest)
  end

  def test_new
    a = Digest::SHA1.new
    b = a.new
    obj = a.to_s
    assert_equal(obj, a.to_s)
    assert_equal(obj, b.to_s)
    a.update('1')
    assert_not_equal(obj, a.to_s)
    assert_equal(obj, b.to_s)
  end

  def test_digest_hexdigest
    [:digest, :hexdigest].each do |m|
      exp_1st = "\3"; exp_1st = Digest.hexencode(exp_1st) if m == :hexdigest
      exp_2nd = "\6"; exp_2nd = Digest.hexencode(exp_2nd) if m == :hexdigest
      digester = @MyDigest.new
      digester.update("foo")
      obj = digester.send(m)
      # digest w/o param does not reset the org digester.
      assert_equal(exp_1st, obj)
      digester.update("bar")
      obj = digester.send(m)
      assert_equal(exp_2nd, obj)
      obj = digester.send(m, "baz")
      # digest with param resets the org digester.
      assert_equal(exp_1st, obj)
    end
  end

  def test_digest_hexdigest_bang
    [:digest!, :hexdigest!].each do |m|
      exp_1st = "\3"; exp_1st = Digest.hexencode(exp_1st) if m == :hexdigest!
      digester = @MyDigest.new
      digester.update("foo")
      obj = digester.send(m) # digest! always resets the org digester.
      assert_equal(exp_1st, obj)
      digester.update("bar")
      obj = digester.send(m)
      assert_equal(exp_1st, obj)
    end
  end

  def test_to_s
    digester = @MyDigest.new
    digester.update("foo")
    assert_equal("03", digester.to_s)
  end

  def test_length
    @MyDigest.class_eval do
      def digest_length
        2
      end
    end
    digester = @MyDigest.new
    assert_equal(2, digester.length)
    assert_equal(2, digester.size)
  end

  def test_digest_length # breaks @MyDigest#digest_length
    assert_equal(1, @MyDigest.new.digest_length)
    @MyDigest.class_eval do
      def digest_length
        2
      end
    end
    assert_equal(2, @MyDigest.new.digest_length)
  end

  def test_block_length
    assert_raises(RuntimeError) do
      @MyDigest.new.block_length
    end
  end
end
