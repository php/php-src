#
#   help.rb - helper using ri
#   	$Release Version: 0.9.6$
#   	$Revision$
#
# --
#
#
#

require 'rdoc/ri/driver'

require "irb/cmd/nop.rb"

module IRB
  module ExtendCommand
    class Help<Nop
      begin
        Ri = RDoc::RI::Driver.new
      rescue SystemExit
      else
        def execute(*names)
          if names.empty?
            Ri.interactive
            return
          end
          names.each do |name|
            begin
              Ri.display_name(name.to_s)
            rescue RDoc::RI::Error
              puts $!.message
            end
          end
          nil
        end
      end
    end
  end
end
