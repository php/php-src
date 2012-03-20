require 'drb/drb'
require 'drb/unix'

class Foo
  include DRbUndumped

  def foo(n)
    n + n
  end

  def bar(n)
    yield(n) + yield(n)
  end
end

DRb.start_service('drubyunix:', nil)
puts DRb.uri

ro = DRbObject.new(nil, ARGV.shift)
ro[:unix] = Foo.new
gets

it = ro[:tcp]
p [it, it.foo(1)]
gets

p it.bar('2') {|n| n * 3}
gets
