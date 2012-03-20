require 'rake'

module Rake

  # Base class for Task Libraries.
  class TaskLib
    include Cloneable
    include Rake::DSL

    # Make a symbol by pasting two strings together.
    #
    # NOTE: DEPRECATED! This method is kinda stupid. I don't know why
    # I didn't just use string interpolation. But now other task
    # libraries depend on this so I can't remove it without breaking
    # other people's code. So for now it stays for backwards
    # compatibility. BUT DON'T USE IT.
    def paste(a,b)              # :nodoc:
      (a.to_s + b.to_s).intern
    end
  end

end
