require 'drb/drb'
require 'drb/extserv'
require 'timeout'

class XArray < Array
  def initialize(ary)
    ary.each do |x|
      self.push(x)
    end
  end
end

class XArray2 < XArray
  include DRbUndumped
end

class Unknown2
  def initialize
    @foo = 'unknown2'
  end
end

class DRbEx
  include DRbUndumped

  class FooBar
    def initialize
      @foo = 'bar'
    end
  end

  class UError < RuntimeError; end

  def initialize
    @hello = 'hello'
  end
  attr_reader :hello

  def sample(a, b, c)
    a.to_i + b.to_i + c.to_i
  end

  def sum(*a)
    s = 0
    a.each do |e|
      s += e.to_i
    end
    s
  end

  def do_timeout(n)
    timeout(0.1) do
      n.sleep(2)
    end
  end

  def unknown_module
    FooBar.new
  end

  def unknown_class
    Unknown2.new
  end

  def unknown_error
    raise UError
  end

  def remote_no_method_error
    invoke_no_method(self)
  end

  def test_yield
    yield
    yield([])
    yield(*[])
  end

  def echo_yield(*arg)
    yield(*arg)
    nil
  end

  def echo_yield_0
    yield
    nil
  end

  def echo_yield_1(one)
    yield(one)
    nil
  end

  def echo_yield_2(one, two)
    yield(one, two)
    nil
  end

  def xarray_each
    xary = [XArray.new([0])]
    xary.each do |x|
      yield(x)
    end
    nil
  end

  def xarray2_hash
    unless @xary2_hash
      @xary2_hash = { "a" => XArray2.new([0]), "b" => XArray2.new([1]) }
    end
    DRbObject.new(@xary2_hash)
  end

  def simple_hash
    unless @hash
      @hash = { 'a'=>:a, 'b'=>:b }
    end
    DRbObject.new(@hash)
  end

  def [](key)
    key.to_s
  end

  def to_a
    [self]
  end

  def method_missing(msg, *a, &b)
    if msg == :missing
      return true
    else
      super(msg, *a, &b)
    end
  end

  private
  def call_private_method
    true
  end

  protected
  def call_protected_method
    true
  end
end

if __FILE__ == $0
  def ARGV.shift
    it = super()
    raise "usage: #{$0} <manager-uri> <name>" unless it
    it
  end

  DRb::DRbServer.default_argc_limit(8)
  DRb::DRbServer.default_load_limit(4096)
  DRb.start_service('druby://localhost:0', DRbEx.new)
  es = DRb::ExtServ.new(ARGV.shift, ARGV.shift)
  DRb.thread.join
end
