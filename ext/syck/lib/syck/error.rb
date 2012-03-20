#
# Error messages and exception class
#

module Syck

	#
	# Error messages
	#

	ERROR_NO_HEADER_NODE = "With UseHeader=false, the node Array or Hash must have elements"
	ERROR_NEED_HEADER = "With UseHeader=false, the node must be an Array or Hash"
	ERROR_BAD_EXPLICIT = "Unsupported explicit transfer: '%s'"
    ERROR_MANY_EXPLICIT = "More than one explicit transfer"
	ERROR_MANY_IMPLICIT = "More than one implicit request"
	ERROR_NO_ANCHOR = "No anchor for alias '%s'"
	ERROR_BAD_ANCHOR = "Invalid anchor: %s"
	ERROR_MANY_ANCHOR = "More than one anchor"
	ERROR_ANCHOR_ALIAS = "Can't define both an anchor and an alias"
	ERROR_BAD_ALIAS = "Invalid alias: %s"
	ERROR_MANY_ALIAS = "More than one alias"
	ERROR_ZERO_INDENT = "Can't use zero as an indentation width"
	ERROR_UNSUPPORTED_VERSION = "This release of YAML.rb does not support YAML version %s"
	ERROR_UNSUPPORTED_ENCODING = "Attempt to use unsupported encoding: %s"

	#
	# YAML Error classes
	#

	class Error < StandardError; end
	class ParseError < Error; end
    class TypeError < StandardError; end

end
