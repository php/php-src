#
# tk/clock.rb : methods for clock command
#
require 'tk'

module Tk
  module Clock
    include Tk
    extend TkCore

    def self.add(clk, *args)
      tk_call_without_enc('clock','add', clk, *args).to_i
    end

    def self.clicks(ms=nil)
      ms = ms.to_s if ms.kind_of?(Symbol)
      case ms
      when nil, ''
        tk_call_without_enc('clock','clicks').to_i
      when /^mic/
        tk_call_without_enc('clock','clicks','-microseconds').to_i
      when /^mil/
        tk_call_without_enc('clock','clicks','-milliseconds').to_i
      else
        tk_call_without_enc('clock','clicks','-milliseconds').to_i
      end
    end

    def self.format(clk, form=nil)
      if form
        tk_call('clock','format',clk,'-format',form)
      else
        tk_call('clock','format',clk)
      end
    end

    def self.formatGMT(clk, form=nil)
      if form
        tk_call('clock','format',clk,'-format',form,'-gmt','1')
      else
        tk_call('clock','format',clk,'-gmt','1')
      end
    end

    def self.scan(str, base=nil)
      if base
        tk_call('clock','scan',str,'-base',base).to_i
      else
        tk_call('clock','scan',str).to_i
      end
    end

    def self.scanGMT(str, base=nil)
      if base
        tk_call('clock','scan',str,'-base',base,'-gmt','1').to_i
      else
        tk_call('clock','scan',str,'-gmt','1').to_i
      end
    end

    def self.seconds
      tk_call_without_enc('clock','seconds').to_i
    end
    def self.milliseconds
      tk_call_without_enc('clock','milliseconds').to_i
    end
    def self.microseconds
      tk_call_without_enc('clock','microseconds').to_i
    end
  end
end
