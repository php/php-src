#
#   irb/input-method.rb - input methods used irb
#   	$Release Version: 0.9.6$
#   	$Revision$
#   	by Keiju ISHITSUKA(keiju@ruby-lang.org)
#
# --
#
#
#
require 'irb/src_encoding'
require 'irb/magic-file'

module IRB
  #
  # InputMethod
  #	StdioInputMethod
  #	FileInputMethod
  #	(ReadlineInputMethod)
  #
  STDIN_FILE_NAME = "(line)"
  class InputMethod
    @RCS_ID='-$Id$-'

    def initialize(file = STDIN_FILE_NAME)
      @file_name = file
    end
    attr_reader :file_name

    attr_accessor :prompt

    def gets
      IRB.fail NotImplementedError, "gets"
    end
    public :gets

    def readable_atfer_eof?
      false
    end
  end

  class StdioInputMethod < InputMethod
    def initialize
      super
      @line_no = 0
      @line = []
      @stdin = IO.open(STDIN.to_i, :external_encoding => IRB.conf[:LC_MESSAGES].encoding, :internal_encoding => "-")
      @stdout = IO.open(STDOUT.to_i, 'w', :external_encoding => IRB.conf[:LC_MESSAGES].encoding, :internal_encoding => "-")
    end

    def gets
      print @prompt
      line = @stdin.gets
      @line[@line_no += 1] = line
    end

    def eof?
      @stdin.eof?
    end

    def readable_atfer_eof?
      true
    end

    def line(line_no)
      @line[line_no]
    end

    def encoding
      @stdin.external_encoding
    end
  end

  class FileInputMethod < InputMethod
    def initialize(file)
      super
      @io = IRB::MagicFile.open(file)
    end
    attr_reader :file_name

    def eof?
      @io.eof?
    end

    def gets
      print @prompt
      l = @io.gets
#      print @prompt, l
      l
    end

    def encoding
      @io.external_encoding
    end
  end

  begin
    require "readline"
    class ReadlineInputMethod < InputMethod
      include Readline
      def initialize
	super

	@line_no = 0
	@line = []
	@eof = false

	@stdin = IO.open(STDIN.to_i, :external_encoding => IRB.conf[:LC_MESSAGES].encoding, :internal_encoding => "-")
	@stdout = IO.open(STDOUT.to_i, 'w', :external_encoding => IRB.conf[:LC_MESSAGES].encoding, :internal_encoding => "-")
      end

      def gets
        Readline.input = @stdin
        Readline.output = @stdout
	if l = readline(@prompt, false)
	  HISTORY.push(l) if !l.empty?
	  @line[@line_no += 1] = l + "\n"
	else
	  @eof = true
	  l
	end
      end

      def eof?
	@eof
      end

      def readable_atfer_eof?
	true
      end

      def line(line_no)
	@line[line_no]
      end

      def encoding
	@stdin.external_encoding
      end
    end
  rescue LoadError
  end
end
