require 'rdoc'

##
# Namespace for the ri command line tool's implementation.
#
# See <tt>ri --help</tt> for details.

module RDoc::RI

  ##
  # Base RI error class

  class Error < RDoc::Error; end

end

require 'rdoc/ri/store'

