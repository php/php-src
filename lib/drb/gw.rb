require 'drb/drb'
require 'monitor'

module DRb
  class GWIdConv < DRbIdConv
    def to_obj(ref)
      if Array === ref && ref[0] == :DRbObject
        return DRbObject.new_with(ref[1], ref[2])
      end
      super(ref)
    end
  end

  class GW
    include MonitorMixin
    def initialize
      super()
      @hash = {}
    end

    def [](key)
      synchronize do
        @hash[key]
      end
    end

    def []=(key, v)
      synchronize do
        @hash[key] = v
      end
    end
  end

  class DRbObject
    def self._load(s)
      uri, ref = Marshal.load(s)
      if DRb.uri == uri
        return ref ? DRb.to_obj(ref) : DRb.front
      end

      self.new_with(DRb.uri, [:DRbObject, uri, ref])
    end

    def _dump(lv)
      if DRb.uri == @uri
        if Array === @ref && @ref[0] == :DRbObject
          Marshal.dump([@ref[1], @ref[2]])
        else
          Marshal.dump([@uri, @ref]) # ??
        end
      else
        Marshal.dump([DRb.uri, [:DRbObject, @uri, @ref]])
      end
    end
  end
end

=begin
DRb.install_id_conv(DRb::GWIdConv.new)

front = DRb::GW.new

s1 = DRb::DRbServer.new('drbunix:/tmp/gw_b_a', front)
s2 = DRb::DRbServer.new('drbunix:/tmp/gw_b_c', front)

s1.thread.join
s2.thread.join
=end

=begin
# foo.rb

require 'drb/drb'

class Foo
  include DRbUndumped
  def initialize(name, peer=nil)
    @name = name
    @peer = peer
  end

  def ping(obj)
    puts "#{@name}: ping: #{obj.inspect}"
    @peer.ping(self) if @peer
  end
end
=end

=begin
# gw_a.rb
require 'drb/unix'
require 'foo'

obj = Foo.new('a')
DRb.start_service("drbunix:/tmp/gw_a", obj)

robj = DRbObject.new_with_uri('drbunix:/tmp/gw_b_a')
robj[:a] = obj

DRb.thread.join
=end

=begin
# gw_c.rb
require 'drb/unix'
require 'foo'

foo = Foo.new('c', nil)

DRb.start_service("drbunix:/tmp/gw_c", nil)

robj = DRbObject.new_with_uri("drbunix:/tmp/gw_b_c")

puts "c->b"
a = robj[:a]
sleep 2

a.ping(foo)

DRb.thread.join
=end

