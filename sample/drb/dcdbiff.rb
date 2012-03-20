#
# dcdbiff.rb - distributed cdbiff (client)
#  * original: cdbiff by Satoru Takabayashi <http://namazu.org/~satoru/cdbiff>

require 'drb/drb'
require 'drb/eq'

class Notify
  include DRbUndumped

  def initialize(biff, command)
    @biff = biff
    @command = command

    @biff.add_observer(self)
  end

  def update(filename, time)
    p [filename, time] if $DEBUG
    system(@command)
  end

  def done
    begin
      @biff.delete_observer(self)
    rescue
    end
  end
end

def main
  command = 'eject'
  uri = 'druby://localhost:19903'

  DRb.start_service
  biff = DRbObject.new(nil, uri)
  notify = Notify.new(biff, command)

  trap("INT"){ notify.done }
  DRb.thread.join
end

main
