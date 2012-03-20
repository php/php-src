require 'drb/drb'

class Foo
  include DRbUndumped

  def foo(n)
    n + n
  end

  def bar(n)
    yield(n) + yield(n)
  end
end

DRb.start_service(nil)
puts DRb.uri

ro = DRbObject.new(nil, ARGV.shift)
ro[:tcp] = Foo.new
gets

it = ro[:unix]
p [it, it.foo(1)]
gets

p it.bar('2') {|n| n * 3}
gets


