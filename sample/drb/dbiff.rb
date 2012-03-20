#
# dbiff.rb - distributed cdbiff (server)
#  * original: cdbiff by Satoru Takabayashi <http://namazu.org/~satoru/cdbiff>

require 'drb/drb'
require 'drb/eq'
require 'drb/observer'

class Biff
  include DRb::DRbObservable

  def initialize(filename, interval)
    super()
    @filename = filename
    @interval = interval
  end

  def run
    last = Time.now
    while true
      begin
	sleep(@interval)
	current = File::mtime(@filename)
	if current > last
	  changed
	  begin
	    notify_observers(@filename, current)
	  rescue Error
	  end
	  last = current
	end
      rescue
	next
      end
    end
  end
end

def main
  filename = "/var/mail/#{ENV['USER']}"
  interval = 15
  uri = 'druby://:19903'

  biff = Biff.new(filename, interval)

  DRb.start_service(uri, biff)
  biff.run
end

main

