require 'test/unit'
require 'drb/drb'
require 'drb/extservm'
require 'timeout'
require 'shellwords'
require_relative '../ruby/envutil'

class DRbService
  @@manager = DRb::ExtServManager.new
  @@ruby = Shellwords.escape(EnvUtil.rubybin)
  @@ruby += " -d" if $DEBUG
  def self.add_service_command(nm)
    dir = File.dirname(File.expand_path(__FILE__))
    DRb::ExtServManager.command[nm] = "#{@@ruby} \"#{dir}/#{nm}\""
  end

  %w(ut_drb.rb ut_array.rb ut_port.rb ut_large.rb ut_safe1.rb ut_eval.rb ut_eq.rb).each do |nm|
    add_service_command(nm)
  end
  @server = @@server = DRb::DRbServer.new('druby://localhost:0', @@manager, {})
  @@manager.uri = @@server.uri
  def self.manager
    @@manager
  end
  def self.server
    @server || @@server
  end
  def self.ext_service(name)
    timeout(100, RuntimeError) do
      manager.service(name)
    end
  end
end

class Onecky
  include DRbUndumped
  def initialize(n)
    @num = n
  end

  def to_i
    @num.to_i
  end

  def sleep(n)
    Kernel.sleep(n)
    to_i
  end
end

class FailOnecky < Onecky
  class OneckyError < RuntimeError; end
  def to_i
    raise(OneckyError, @num.to_s)
  end
end

class XArray < Array
  def initialize(ary)
    ary.each do |x|
      self.push(x)
    end
  end
end

module DRbCore
  def setup
    @ext = DRbService.ext_service('ut_drb.rb')
    @there = @ext.front
  end

  def teardown
    @ext.stop_service if @ext
  end

  def test_00_DRbObject
    ro = DRbObject.new(nil, 'druby://localhost:12345')
    assert_equal('druby://localhost:12345', ro.__drburi)
    assert_equal(nil, ro.__drbref)

    ro = DRbObject.new_with_uri('druby://localhost:12345')
    assert_equal('druby://localhost:12345', ro.__drburi)
    assert_equal(nil, ro.__drbref)

    ro = DRbObject.new_with_uri('druby://localhost:12345?foobar')
    assert_equal('druby://localhost:12345', ro.__drburi)
    assert_equal(DRb::DRbURIOption.new('foobar'), ro.__drbref)
  end

  def test_01
    assert_equal("hello", @there.hello)
    onecky = Onecky.new('3')
    assert_equal(6, @there.sample(onecky, 1, 2))
    ary = @there.to_a
    assert_kind_of(DRb::DRbObject, ary)

    assert(@there.respond_to?(:to_a, true))
    assert(@there.respond_to?(:eval, true))
    assert(! @there.respond_to?(:eval, false))
    assert(! @there.respond_to?(:eval))
  end

  def test_01_02_loop
    onecky = Onecky.new('3')
    50.times do
      assert_equal(6, @there.sample(onecky, 1, 2))
      ary = @there.to_a
      assert_kind_of(DRb::DRbObject, ary)
    end
  end

  def test_02_unknown
    obj = @there.unknown_class
    assert_kind_of(DRb::DRbUnknown, obj)
    assert_equal('Unknown2', obj.name)

    obj = @there.unknown_module
    assert_kind_of(DRb::DRbUnknown, obj)
    if RUBY_VERSION >= '1.8'
      assert_equal('DRbEx::', obj.name)
    else
      assert_equal('DRbEx', obj.name)
    end

    assert_raise(DRb::DRbUnknownError) do
      @there.unknown_error
    end

    onecky = FailOnecky.new('3')

    assert_raise(FailOnecky::OneckyError) do
      @there.sample(onecky, 1, 2)
    end
  end

  def test_03
    assert_equal(8, @there.sum(1, 1, 1, 1, 1, 1, 1, 1))
    assert_raise(DRb::DRbConnError) do
      @there.sum(1, 1, 1, 1, 1, 1, 1, 1, 1)
    end
    assert_raise(DRb::DRbConnError) do
      @there.sum('1' * 4096)
    end
  end

  def test_04
    assert_respond_to(@there, 'sum')
    assert(!(@there.respond_to? "foobar"))
  end

  def test_05_eq
    a = @there.to_a[0]
    b = @there.to_a[0]
    assert(a.object_id != b.object_id)
    assert(a == b)
    assert_equal(a, b)
    assert(a == @there)
    assert_equal(a.hash, b.hash)
    assert_equal(a.hash, @there.hash)
    assert(a.eql?(b))
    assert(a.eql?(@there))
  end

  def test_06_timeout
    ten = Onecky.new(10)
    assert_raise(TimeoutError) do
      @there.do_timeout(ten)
    end
    assert_raise(TimeoutError) do
      @there.do_timeout(ten)
    end
  end

  def test_07_public_private_protected_missing
    assert_nothing_raised() {
      begin
	@there.method_missing(:eval, 'nil')
      rescue NoMethodError
	assert_match(/^private method \`eval\'/, $!.message)
      end
    }
    assert_nothing_raised() {
      begin
        @there.call_private_method
      rescue NoMethodError
        assert_equal(NoMethodError, $!.class)
	assert_match(/^private method \`call_private_method\'/, $!.message)
      end
    }
    assert_nothing_raised() {
      begin
        @there.call_protected_method
      rescue NoMethodError
        assert_equal(NoMethodError, $!.class)
	assert_match(/^protected method \`call_protected_method\'/, $!.message)
      end
    }
    assert_nothing_raised() {
      begin
	@there.method_missing(:undefined_method_test)
      rescue NoMethodError
        assert_equal(NoMethodError, $!.class)
	assert_match(/^undefined method \`undefined_method_test\'/, $!.message)
      end
    }
    assert_raise(DRb::DRbConnError) do
      @there.method_missing(:__send__, :to_s)
    end
    assert_equal(true, @there.missing)
  end

  def test_08_here
    ro = DRbObject.new(nil, DRb.uri)
    assert_kind_of(String, ro.to_s)

    ro = DRbObject.new_with_uri(DRb.uri)
    assert_kind_of(String, ro.to_s)
  end

  def uri_concat_option(uri, opt)
    "#{uri}?#{opt}"
  end

  def test_09_option
    uri = uri_concat_option(@there.__drburi, "foo")
    ro = DRbObject.new_with_uri(uri)
    assert_equal(ro.__drburi, @there.__drburi)
    assert_equal(3, ro.size)

    uri = uri_concat_option(@there.__drburi, "")
    ro = DRbObject.new_with_uri(uri)
    assert_equal(ro.__drburi, @there.__drburi)
    assert_equal(DRb::DRbURIOption.new(''), ro.__drbref)

    uri = uri_concat_option(@there.__drburi, "hello?world")
    ro = DRbObject.new_with_uri(uri)
    assert_equal(DRb::DRbURIOption.new('hello?world'), ro.__drbref)

    uri = uri_concat_option(@there.__drburi, "?hello?world")
    ro = DRbObject.new_with_uri(uri)
    assert_equal(DRb::DRbURIOption.new('?hello?world'), ro.__drbref)
  end

  def test_10_yield
    @there.simple_hash.each do |k, v|
      assert_kind_of(String, k)
      assert_kind_of(Symbol, v)
    end
  end

  def test_10_yield_undumped
    @there.xarray2_hash.each do |k, v|
      assert_kind_of(String, k)
      assert_kind_of(DRbObject, v)
    end
  end

  def test_11_remote_no_method_error
    assert_raise(DRb::DRbRemoteError) do
      @there.remote_no_method_error
    end
    begin
      @there.remote_no_method_error
    rescue
      error = $!
      assert_match(/^undefined method .*\(NoMethodError\)/, error.message)
      assert_equal('NoMethodError', error.reason)
    end
  end
end

module DRbAry
  def setup
    @ext = DRbService.ext_service('ut_array.rb')
    @there = @ext.front
  end

  def teardown
    @ext.stop_service if @ext
  end

  def test_01
    assert_kind_of(DRb::DRbObject, @there)
  end

  def test_02_collect
    ary = @there.collect do |x| x + x end
    assert_kind_of(Array, ary)
    assert_equal([2, 4, 'IIIIII', 8, 'fivefive', 12], ary)
  end

  def test_03_redo
    ary = []
    count = 0
    @there.each do |x|
      count += 1
      ary.push x
      redo if count == 3
    end
    assert_equal([1, 2, 'III', 'III', 4, 'five', 6], ary)
  end

  # retry in block is not supported on ruby 1.9
  #def test_04_retry
  #  retried = false
  #  ary = []
  #  @there.each do |x|
  #    ary.push x
  #    if x == 4 && !retried
  #	retried = true
  #	retry
  #    end
  #  end
  #  assert_equal([1, 2, 'III', 4, 1, 2, 'III', 4, 'five', 6], ary)
  #end

  def test_05_break
    ary = []
    @there.each do |x|
      ary.push x
      break if x == 4
    end
    assert_equal([1, 2, 'III', 4], ary)
  end

  def test_06_next
    ary = []
    @there.each do |x|
      next if String === x
      ary.push x
    end
    assert_equal([1, 2, 4, 6], ary)
  end

  class_eval <<EOS
  def test_07_break_18
    ary = []
    result = @there.each do |x|
      ary.push x
      break(:done) if x == 4
    end
    assert_equal([1, 2, 'III', 4], ary)
    assert_equal(:done, result)
  end
EOS

end
