=begin
 distributed Ruby --- NamedObject Sample
 	Copyright (c) 2000-2001 Masatoshi SEKI
=end

=begin
How to play.

* start server
 Terminal 1
 | % ruby name.rb druby://yourhost:7640
 | druby://yourhost:7640
 | [return] to exit

* start client
 Terminal 2
 | % ruby namec.rb druby://yourhost:7640
 | #<DRb::DRbObject:0x40164174 @uri="druby://yourhost:7640", @ref="seq">
 | #<DRb::DRbObject:0x40163c9c @uri="druby://yourhost:7640", @ref="mutex">
 | 1
 | 2
 | [return] to continue

* restart server
 Terminal 1
 type [return]
 | % ruby name.rb druby://yourhost:7640
 | druby://yourhost:7640
 | [return] to exit

* continue client
 Terminal 2
 type [return]
 | 1
 | 2
=end

require 'thread.rb'
require 'drb/drb'

module DRbNamedObject
  DRbNAMEDICT = {}
  attr_reader(:drb_name)

  def drb_name=(name)
    @drb_name = name
    Thread.exclusive do
      raise(IndexError, name) if DRbNAMEDICT[name]
      DRbNAMEDICT[name] = self
    end
  end
end

class DRbNamedIdConv < DRb::DRbIdConv
  def initialize
    @dict = DRbNamedObject::DRbNAMEDICT
  end

  def to_obj(ref)
    @dict.fetch(ref) do super end
  end

  def to_id(obj)
    if obj.kind_of? DRbNamedObject
      return obj.drb_name
    else
      return super
    end
  end
end

class Seq
  include DRbUndumped
  include DRbNamedObject

  def initialize(v, name)
    @counter = v
    @mutex = Mutex.new
    self.drb_name = name
  end

  def next_value
    @mutex.synchronize do
      @counter += 1
      return @counter
    end
  end
end

class Front
  def initialize
    seq = Seq.new(0, 'seq')
    mutex = Mutex.new
    mutex.extend(DRbUndumped)
    mutex.extend(DRbNamedObject)
    mutex.drb_name = 'mutex'
    @name = {}
    @name['seq'] = seq
    @name['mutex'] = mutex
  end

  def [](k)
    @name[k]
  end
end

if __FILE__ == $0
  uri = ARGV.shift

  name_conv = DRbNamedIdConv.new

  DRb.install_id_conv(name_conv)
  DRb.start_service(uri, Front.new)
  puts DRb.uri
  DRb.thread.join
end

