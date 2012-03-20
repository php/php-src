#!/usr/local/bin/ruby
#
#   irbtk.rb - irb with Ruby/Tk
#
#   If you want to use 'multi-tk.rb', give option '--multi-tk'.
#   And if you want to use 'remote-tk.rb', give option '--remote-tk'.
#   If you want both, you don't need to give both options, because
#   'remote-tk.rb' includes 'multi-tk.rb'.
#   ( There is no trouble even if you give both options. )
#
require 'remote-tk' if ARGV.delete('--remote-tk')
require 'multi-tk'  if ARGV.delete('--multi-tk')

require "tk"
module Tk
  MAINLOOP = Thread.new{ mainloop }
end

require "irb"

if __FILE__ == $0
  IRB.start(__FILE__)
else
  # check -e option
  if /^-e$/ =~ $0
    IRB.start(__FILE__)
  else
    IRB.setup(__FILE__)
  end
end
