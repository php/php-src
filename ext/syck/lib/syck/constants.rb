#
# Constants used throughout the library
#
module Syck

	#
	# Constants
	#
	VERSION = '0.60'
	SUPPORTED_YAML_VERSIONS = ['1.0']

	#
	# Parser tokens
	#
	WORD_CHAR = 'A-Za-z0-9'
	PRINTABLE_CHAR = '-_A-Za-z0-9!?/()$\'". '
	NOT_PLAIN_CHAR = '\x7f\x0-\x1f\x80-\x9f'
	ESCAPE_CHAR = '[\\x00-\\x09\\x0b-\\x1f]'
	INDICATOR_CHAR = '*&!|\\\\^@%{}[]='
	SPACE_INDICATORS = '-#:,?'
	RESTRICTED_INDICATORS = '#:,}]'
	DNS_COMP_RE = "\\w(?:[-\\w]*\\w)?"
	DNS_NAME_RE = "(?:(?:#{DNS_COMP_RE}\\.)+#{DNS_COMP_RE}|#{DNS_COMP_RE})"
	ESCAPES = %w{\x00   \x01	\x02	\x03	\x04	\x05	\x06	\a
			     \x08	\t		\n		\v		\f		\r		\x0e	\x0f
				 \x10	\x11	\x12	\x13	\x14	\x15	\x16	\x17
				 \x18	\x19	\x1a	\e		\x1c	\x1d	\x1e	\x1f
			    }
	UNESCAPES = {
				'a' => "\x07", 'b' => "\x08", 't' => "\x09",
				'n' => "\x0a", 'v' => "\x0b", 'f' => "\x0c",
				'r' => "\x0d", 'e' => "\x1b", '\\' => '\\',
			    }

	#
	# Default settings
	#
	DEFAULTS = {
		:Indent => 2, :UseHeader => false, :UseVersion => false, :Version => '1.0',
		:SortKeys => false, :AnchorFormat => 'id%03d', :ExplicitTypes => false,
		:WidthType => 'absolute', :BestWidth => 80,
		:UseBlock => false, :UseFold => false, :Encoding => :None
	}

end
