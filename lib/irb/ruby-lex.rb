#
#   irb/ruby-lex.rb - ruby lexcal analyzer
#   	$Release Version: 0.9.6$
#   	$Revision$
#   	by Keiju ISHITSUKA(keiju@ruby-lang.org)
#
# --
#
#
#

require "e2mmap"
require "irb/slex"
require "irb/ruby-token"

class RubyLex
  @RCS_ID='-$Id$-'

  extend Exception2MessageMapper
  def_exception(:AlreadyDefinedToken, "Already defined token(%s)")
  def_exception(:TkReading2TokenNoKey, "key nothing(key='%s')")
  def_exception(:TkSymbol2TokenNoKey, "key nothing(key='%s')")
  def_exception(:TkReading2TokenDuplicateError,
		"key duplicate(token_n='%s', key='%s')")
  def_exception(:SyntaxError, "%s")

  def_exception(:TerminateLineInput, "Terminate Line Input")

  include RubyToken

  class << self
    attr_accessor :debug_level
    def debug?
      @debug_level > 0
    end
  end
  @debug_level = 0

  def initialize
    lex_init
    set_input(STDIN)

    @seek = 0
    @exp_line_no = @line_no = 1
    @base_char_no = 0
    @char_no = 0
    @rests = []
    @readed = []
    @here_readed = []

    @indent = 0
    @indent_stack = []
    @lex_state = EXPR_BEG
    @space_seen = false
    @here_header = false

    @continue = false
    @line = ""

    @skip_space = false
    @readed_auto_clean_up = false
    @exception_on_syntax_error = true

    @prompt = nil
  end

  attr_accessor :skip_space
  attr_accessor :readed_auto_clean_up
  attr_accessor :exception_on_syntax_error

  attr_reader :seek
  attr_reader :char_no
  attr_reader :line_no
  attr_reader :indent

  # io functions
  def set_input(io, p = nil, &block)
    @io = io
    if p.respond_to?(:call)
      @input = p
    elsif block_given?
      @input = block
    else
      @input = Proc.new{@io.gets}
    end
  end

  def get_readed
    if idx = @readed.reverse.index("\n")
      @base_char_no = idx
    else
      @base_char_no += @readed.size
    end

    readed = @readed.join("")
    @readed = []
    readed
  end

  def getc
    while @rests.empty?
#      return nil unless buf_input
      @rests.push nil unless buf_input
    end
    c = @rests.shift
    if @here_header
      @here_readed.push c
    else
      @readed.push c
    end
    @seek += 1
    if c == "\n"
      @line_no += 1
      @char_no = 0
    else
      @char_no += 1
    end
    c
  end

  def gets
    l = ""
    while c = getc
      l.concat(c)
      break if c == "\n"
    end
    return nil if l == "" and c.nil?
    l
  end

  def eof?
    @io.eof?
  end

  def getc_of_rests
    if @rests.empty?
      nil
    else
      getc
    end
  end

  def ungetc(c = nil)
    if @here_readed.empty?
      c2 = @readed.pop
    else
      c2 = @here_readed.pop
    end
    c = c2 unless c
    @rests.unshift c #c =
    @seek -= 1
    if c == "\n"
      @line_no -= 1
      if idx = @readed.reverse.index("\n")
	@char_no = @readed.size - idx
      else
	@char_no = @base_char_no + @readed.size
      end
    else
      @char_no -= 1
    end
  end

  def peek_equal?(str)
    chrs = str.split(//)
    until @rests.size >= chrs.size
      return false unless buf_input
    end
    @rests[0, chrs.size] == chrs
  end

  def peek_match?(regexp)
    while @rests.empty?
      return false unless buf_input
    end
    regexp =~ @rests.join("")
  end

  def peek(i = 0)
    while @rests.size <= i
      return nil unless buf_input
    end
    @rests[i]
  end

  def buf_input
    prompt
    line = @input.call
    return nil unless line
    @rests.concat line.chars.to_a
    true
  end
  private :buf_input

  def set_prompt(p = nil, &block)
    p = block if block_given?
    if p.respond_to?(:call)
      @prompt = p
    else
      @prompt = Proc.new{print p}
    end
  end

  def prompt
    if @prompt
      @prompt.call(@ltype, @indent, @continue, @line_no)
    end
  end

  def initialize_input
    @ltype = nil
    @quoted = nil
    @indent = 0
    @indent_stack = []
    @lex_state = EXPR_BEG
    @space_seen = false
    @here_header = false

    @continue = false
    prompt

    @line = ""
    @exp_line_no = @line_no
  end

  def each_top_level_statement
    initialize_input
    catch(:TERM_INPUT) do
      loop do
	begin
	  @continue = false
	  prompt
	  unless l = lex
	    throw :TERM_INPUT if @line == ''
	  else
	    @line.concat l
	    if @ltype or @continue or @indent > 0
	      next
	    end
	  end
	  if @line != "\n"
            @line.force_encoding(@io.encoding)
	    yield @line, @exp_line_no
	  end
	  break unless l
	  @line = ''
	  @exp_line_no = @line_no

	  @indent = 0
	  @indent_stack = []
	  prompt
	rescue TerminateLineInput
	  initialize_input
	  prompt
	  get_readed
	end
      end
    end
  end

  def lex
    until (((tk = token).kind_of?(TkNL) || tk.kind_of?(TkEND_OF_SCRIPT)) &&
	     !@continue or
	     tk.nil?)
      #p tk
      #p @lex_state
      #p self
    end
    line = get_readed
    #      print self.inspect
    if line == "" and tk.kind_of?(TkEND_OF_SCRIPT) || tk.nil?
      nil
    else
      line
    end
  end

  def token
    #      require "tracer"
    #      Tracer.on
    @prev_seek = @seek
    @prev_line_no = @line_no
    @prev_char_no = @char_no
    begin
      begin
	tk = @OP.match(self)
	@space_seen = tk.kind_of?(TkSPACE)
      rescue SyntaxError
	raise if @exception_on_syntax_error
	tk = TkError.new(@seek, @line_no, @char_no)
      end
    end while @skip_space and tk.kind_of?(TkSPACE)
    if @readed_auto_clean_up
      get_readed
    end
    #      Tracer.off
    tk
  end

  ENINDENT_CLAUSE = [
    "case", "class", "def", "do", "for", "if",
    "module", "unless", "until", "while", "begin" #, "when"
  ]
  DEINDENT_CLAUSE = ["end" #, "when"
  ]

  PERCENT_LTYPE = {
    "q" => "\'",
    "Q" => "\"",
    "x" => "\`",
    "r" => "/",
    "w" => "]",
    "W" => "]",
    "s" => ":"
  }

  PERCENT_PAREN = {
    "{" => "}",
    "[" => "]",
    "<" => ">",
    "(" => ")"
  }

  Ltype2Token = {
    "\'" => TkSTRING,
    "\"" => TkSTRING,
    "\`" => TkXSTRING,
    "/" => TkREGEXP,
    "]" => TkDSTRING,
    ":" => TkSYMBOL
  }
  DLtype2Token = {
    "\"" => TkDSTRING,
    "\`" => TkDXSTRING,
    "/" => TkDREGEXP,
  }

  def lex_init()
    @OP = IRB::SLex.new
    @OP.def_rules("\0", "\004", "\032") do |op, io|
      Token(TkEND_OF_SCRIPT)
    end

    @OP.def_rules(" ", "\t", "\f", "\r", "\13") do |op, io|
      @space_seen = true
      while getc =~ /[ \t\f\r\13]/; end
      ungetc
      Token(TkSPACE)
    end

    @OP.def_rule("#") do |op, io|
      identify_comment
    end

    @OP.def_rule("=begin",
		 proc{|op, io| @prev_char_no == 0 && peek(0) =~ /\s/}) do
      |op, io|
      @ltype = "="
      until getc == "\n"; end
      until peek_equal?("=end") && peek(4) =~ /\s/
	until getc == "\n"; end
      end
      gets
      @ltype = nil
      Token(TkRD_COMMENT)
    end

    @OP.def_rule("\n") do |op, io|
      print "\\n\n" if RubyLex.debug?
      case @lex_state
      when EXPR_BEG, EXPR_FNAME, EXPR_DOT
	@continue = true
      else
	@continue = false
	@lex_state = EXPR_BEG
	until (@indent_stack.empty? ||
	       [TkLPAREN, TkLBRACK, TkLBRACE,
		 TkfLPAREN, TkfLBRACK, TkfLBRACE].include?(@indent_stack.last))
	  @indent_stack.pop
	end
      end
      @here_header = false
      @here_readed = []
      Token(TkNL)
    end

    @OP.def_rules("*", "**",
		  "=", "==", "===",
		  "=~", "<=>",
		  "<", "<=",
		  ">", ">=", ">>",
		  "!", "!=", "!~") do
      |op, io|
      case @lex_state
      when EXPR_FNAME, EXPR_DOT
	@lex_state = EXPR_ARG
      else
	@lex_state = EXPR_BEG
      end
      Token(op)
    end

    @OP.def_rules("<<") do
      |op, io|
      tk = nil
      if @lex_state != EXPR_END && @lex_state != EXPR_CLASS &&
	  (@lex_state != EXPR_ARG || @space_seen)
	c = peek(0)
	if /\S/ =~ c && (/["'`]/ =~ c || /\w/ =~ c || c == "-")
	  tk = identify_here_document
	end
      end
      unless tk
	tk = Token(op)
	case @lex_state
	when EXPR_FNAME, EXPR_DOT
	  @lex_state = EXPR_ARG
	else
	  @lex_state = EXPR_BEG
	end
      end
      tk
    end

    @OP.def_rules("'", '"') do
      |op, io|
      identify_string(op)
    end

    @OP.def_rules("`") do
      |op, io|
      if @lex_state == EXPR_FNAME
	@lex_state = EXPR_END
	Token(op)
      else
	identify_string(op)
      end
    end

    @OP.def_rules('?') do
      |op, io|
      if @lex_state == EXPR_END
	@lex_state = EXPR_BEG
	Token(TkQUESTION)
      else
	ch = getc
	if @lex_state == EXPR_ARG && ch =~ /\s/
	  ungetc
	  @lex_state = EXPR_BEG;
	  Token(TkQUESTION)
	else
	  if (ch == '\\')
	    read_escape
	  end
	  @lex_state = EXPR_END
	  Token(TkINTEGER)
	end
      end
    end

    @OP.def_rules("&", "&&", "|", "||") do
      |op, io|
      @lex_state = EXPR_BEG
      Token(op)
    end

    @OP.def_rules("+=", "-=", "*=", "**=",
		  "&=", "|=", "^=", "<<=", ">>=", "||=", "&&=") do
      |op, io|
      @lex_state = EXPR_BEG
      op =~ /^(.*)=$/
      Token(TkOPASGN, $1)
    end

    @OP.def_rule("+@", proc{|op, io| @lex_state == EXPR_FNAME}) do
      |op, io|
      @lex_state = EXPR_ARG
      Token(op)
    end

    @OP.def_rule("-@", proc{|op, io| @lex_state == EXPR_FNAME}) do
      |op, io|
      @lex_state = EXPR_ARG
      Token(op)
    end

    @OP.def_rules("+", "-") do
      |op, io|
      catch(:RET) do
	if @lex_state == EXPR_ARG
	  if @space_seen and peek(0) =~ /[0-9]/
	    throw :RET, identify_number
	  else
	    @lex_state = EXPR_BEG
	  end
	elsif @lex_state != EXPR_END and peek(0) =~ /[0-9]/
	  throw :RET, identify_number
	else
	  @lex_state = EXPR_BEG
	end
	Token(op)
      end
    end

    @OP.def_rule(".") do
      |op, io|
      @lex_state = EXPR_BEG
      if peek(0) =~ /[0-9]/
	ungetc
	identify_number
      else
	# for "obj.if" etc.
	@lex_state = EXPR_DOT
	Token(TkDOT)
      end
    end

    @OP.def_rules("..", "...") do
      |op, io|
      @lex_state = EXPR_BEG
      Token(op)
    end

    lex_int2
  end

  def lex_int2
    @OP.def_rules("]", "}", ")") do
      |op, io|
      @lex_state = EXPR_END
      @indent -= 1
      @indent_stack.pop
      Token(op)
    end

    @OP.def_rule(":") do
      |op, io|
      if @lex_state == EXPR_END || peek(0) =~ /\s/
	@lex_state = EXPR_BEG
	Token(TkCOLON)
      else
	@lex_state = EXPR_FNAME;
	Token(TkSYMBEG)
      end
    end

    @OP.def_rule("::") do
       |op, io|
#      p @lex_state.id2name, @space_seen
      if @lex_state == EXPR_BEG or @lex_state == EXPR_ARG && @space_seen
	@lex_state = EXPR_BEG
	Token(TkCOLON3)
      else
	@lex_state = EXPR_DOT
	Token(TkCOLON2)
      end
    end

    @OP.def_rule("/") do
      |op, io|
      if @lex_state == EXPR_BEG || @lex_state == EXPR_MID
	identify_string(op)
      elsif peek(0) == '='
	getc
	@lex_state = EXPR_BEG
	Token(TkOPASGN, "/") #/)
      elsif @lex_state == EXPR_ARG and @space_seen and peek(0) !~ /\s/
	identify_string(op)
      else
	@lex_state = EXPR_BEG
	Token("/") #/)
      end
    end

    @OP.def_rules("^") do
      |op, io|
      @lex_state = EXPR_BEG
      Token("^")
    end

    #       @OP.def_rules("^=") do
    # 	@lex_state = EXPR_BEG
    # 	Token(OP_ASGN, :^)
    #       end

    @OP.def_rules(",") do
      |op, io|
      @lex_state = EXPR_BEG
      Token(op)
    end

    @OP.def_rules(";") do
      |op, io|
      @lex_state = EXPR_BEG
      until (@indent_stack.empty? ||
	     [TkLPAREN, TkLBRACK, TkLBRACE,
	       TkfLPAREN, TkfLBRACK, TkfLBRACE].include?(@indent_stack.last))
	@indent_stack.pop
      end
      Token(op)
    end

    @OP.def_rule("~") do
      |op, io|
      @lex_state = EXPR_BEG
      Token("~")
    end

    @OP.def_rule("~@", proc{|op, io| @lex_state == EXPR_FNAME}) do
      |op, io|
      @lex_state = EXPR_BEG
      Token("~")
    end

    @OP.def_rule("(") do
      |op, io|
      @indent += 1
      if @lex_state == EXPR_BEG || @lex_state == EXPR_MID
	@lex_state = EXPR_BEG
	tk_c = TkfLPAREN
      else
	@lex_state = EXPR_BEG
	tk_c = TkLPAREN
      end
      @indent_stack.push tk_c
      Token(tk_c)
    end

    @OP.def_rule("[]", proc{|op, io| @lex_state == EXPR_FNAME}) do
      |op, io|
      @lex_state = EXPR_ARG
      Token("[]")
    end

    @OP.def_rule("[]=", proc{|op, io| @lex_state == EXPR_FNAME}) do
      |op, io|
      @lex_state = EXPR_ARG
      Token("[]=")
    end

    @OP.def_rule("[") do
      |op, io|
      @indent += 1
      if @lex_state == EXPR_FNAME
	tk_c = TkfLBRACK
      else
	if @lex_state == EXPR_BEG || @lex_state == EXPR_MID
	  tk_c = TkLBRACK
	elsif @lex_state == EXPR_ARG && @space_seen
	  tk_c = TkLBRACK
	else
	  tk_c = TkfLBRACK
	end
	@lex_state = EXPR_BEG
      end
      @indent_stack.push tk_c
      Token(tk_c)
    end

    @OP.def_rule("{") do
      |op, io|
      @indent += 1
      if @lex_state != EXPR_END && @lex_state != EXPR_ARG
	tk_c = TkLBRACE
      else
	tk_c = TkfLBRACE
      end
      @lex_state = EXPR_BEG
      @indent_stack.push tk_c
      Token(tk_c)
    end

    @OP.def_rule('\\') do
      |op, io|
      if getc == "\n"
	@space_seen = true
	@continue = true
	Token(TkSPACE)
      else
	read_escape
	Token("\\")
      end
    end

    @OP.def_rule('%') do
      |op, io|
      if @lex_state == EXPR_BEG || @lex_state == EXPR_MID
	identify_quotation
      elsif peek(0) == '='
	getc
	Token(TkOPASGN, :%)
      elsif @lex_state == EXPR_ARG and @space_seen and peek(0) !~ /\s/
	identify_quotation
      else
	@lex_state = EXPR_BEG
	Token("%") #))
      end
    end

    @OP.def_rule('$') do
      |op, io|
      identify_gvar
    end

    @OP.def_rule('@') do
      |op, io|
      if peek(0) =~ /[\w@]/
	ungetc
	identify_identifier
      else
	Token("@")
      end
    end

    #       @OP.def_rule("def", proc{|op, io| /\s/ =~ io.peek(0)}) do
    # 	|op, io|
    # 	@indent += 1
    # 	@lex_state = EXPR_FNAME
    # #	@lex_state = EXPR_END
    # #	until @rests[0] == "\n" or @rests[0] == ";"
    # #	  rests.shift
    # #	end
    #       end

    @OP.def_rule("") do
      |op, io|
      printf "MATCH: start %s: %s\n", op, io.inspect if RubyLex.debug?
      if peek(0) =~ /[0-9]/
	t = identify_number
      elsif peek(0) =~ /[^\x00-\/:-@\[-^`{-\x7F]/
	t = identify_identifier
      end
      printf "MATCH: end %s: %s\n", op, io.inspect if RubyLex.debug?
      t
    end

    p @OP if RubyLex.debug?
  end

  def identify_gvar
    @lex_state = EXPR_END

    case ch = getc
    when /[~_*$?!@\/\\;,=:<>".]/   #"
      Token(TkGVAR, "$" + ch)
    when "-"
      Token(TkGVAR, "$-" + getc)
    when "&", "`", "'", "+"
      Token(TkBACK_REF, "$"+ch)
    when /[1-9]/
      while getc =~ /[0-9]/; end
      ungetc
      Token(TkNTH_REF)
    when /\w/
      ungetc
      ungetc
      identify_identifier
    else
      ungetc
      Token("$")
    end
  end

  def identify_identifier
    token = ""
    if peek(0) =~ /[$@]/
      token.concat(c = getc)
      if c == "@" and peek(0) == "@"
	token.concat getc
      end
    end

    while (ch = getc) =~ /[^\x00-\/:-@\[-^`{-\x7F]/
      print ":", ch, ":" if RubyLex.debug?
      token.concat ch
    end
    ungetc

    if (ch == "!" || ch == "?") && token[0,1] =~ /\w/ && peek(0) != "="
      token.concat getc
    end

    # almost fix token

    case token
    when /^\$/
      return Token(TkGVAR, token)
    when /^\@\@/
      @lex_state = EXPR_END
      # p Token(TkCVAR, token)
      return Token(TkCVAR, token)
    when /^\@/
      @lex_state = EXPR_END
      return Token(TkIVAR, token)
    end

    if @lex_state != EXPR_DOT
      print token, "\n" if RubyLex.debug?

      token_c, *trans = TkReading2Token[token]
      if token_c
	# reserved word?

	if (@lex_state != EXPR_BEG &&
	    @lex_state != EXPR_FNAME &&
	    trans[1])
	  # modifiers
	  token_c = TkSymbol2Token[trans[1]]
	  @lex_state = trans[0]
	else
	  if @lex_state != EXPR_FNAME
	    if ENINDENT_CLAUSE.include?(token)
	      # check for ``class = val'' etc.
	      valid = true
	      case token
	      when "class"
		valid = false unless peek_match?(/^\s*(<<|\w|::)/)
	      when "def"
		valid = false if peek_match?(/^\s*(([+\-\/*&\|^]|<<|>>|\|\||\&\&)=|\&\&|\|\|)/)
	      when "do"
		valid = false if peek_match?(/^\s*([+\-\/*]?=|\*|<|>|\&)/)
	      when *ENINDENT_CLAUSE
		valid = false if peek_match?(/^\s*([+\-\/*]?=|\*|<|>|\&|\|)/)
	      else
		# no nothing
	      end
	      if valid
		if token == "do"
		  if ![TkFOR, TkWHILE, TkUNTIL].include?(@indent_stack.last)
		    @indent += 1
		    @indent_stack.push token_c
		  end
		else
		  @indent += 1
		  @indent_stack.push token_c
		end
#		p @indent_stack
	      end

	    elsif DEINDENT_CLAUSE.include?(token)
	      @indent -= 1
	      @indent_stack.pop
	    end
	    @lex_state = trans[0]
	  else
	    @lex_state = EXPR_END
	  end
	end
	return Token(token_c, token)
      end
    end

    if @lex_state == EXPR_FNAME
      @lex_state = EXPR_END
      if peek(0) == '='
	token.concat getc
      end
    elsif @lex_state == EXPR_BEG || @lex_state == EXPR_DOT
      @lex_state = EXPR_ARG
    else
      @lex_state = EXPR_END
    end

    if token[0, 1] =~ /[A-Z]/
      return Token(TkCONSTANT, token)
    elsif token[token.size - 1, 1] =~ /[!?]/
      return Token(TkFID, token)
    else
      return Token(TkIDENTIFIER, token)
    end
  end

  def identify_here_document
    ch = getc
#    if lt = PERCENT_LTYPE[ch]
    if ch == "-"
      ch = getc
      indent = true
    end
    if /['"`]/ =~ ch
      lt = ch
      quoted = ""
      while (c = getc) && c != lt
	quoted.concat c
      end
    else
      lt = '"'
      quoted = ch.dup
      while (c = getc) && c =~ /\w/
	quoted.concat c
      end
      ungetc
    end

    ltback, @ltype = @ltype, lt
    reserve = []
    while ch = getc
      reserve.push ch
      if ch == "\\"
	reserve.push ch = getc
      elsif ch == "\n"
	break
      end
    end

    @here_header = false
    while l = gets
      l = l.sub(/(:?\r)?\n\z/, '')
      if (indent ? l.strip : l) == quoted
 	break
      end
    end

    @here_header = true
    @here_readed.concat reserve
    while ch = reserve.pop
      ungetc ch
    end

    @ltype = ltback
    @lex_state = EXPR_END
    Token(Ltype2Token[lt])
  end

  def identify_quotation
    ch = getc
    if lt = PERCENT_LTYPE[ch]
      ch = getc
    elsif ch =~ /\W/
      lt = "\""
    else
      RubyLex.fail SyntaxError, "unknown type of %string"
    end
#     if ch !~ /\W/
#       ungetc
#       next
#     end
    #@ltype = lt
    @quoted = ch unless @quoted = PERCENT_PAREN[ch]
    identify_string(lt, @quoted)
  end

  def identify_number
    @lex_state = EXPR_END

    if peek(0) == "0" && peek(1) !~ /[.eE]/
      getc
      case peek(0)
      when /[xX]/
	ch = getc
	match = /[0-9a-fA-F_]/
      when /[bB]/
	ch = getc
	match = /[01_]/
      when /[oO]/
	ch = getc
	match = /[0-7_]/
      when /[dD]/
	ch = getc
	match = /[0-9_]/
      when /[0-7]/
	match = /[0-7_]/
      when /[89]/
	RubyLex.fail SyntaxError, "Invalid octal digit"
      else
	return Token(TkINTEGER)
      end

      len0 = true
      non_digit = false
      while ch = getc
	if match =~ ch
	  if ch == "_"
	    if non_digit
	      RubyLex.fail SyntaxError, "trailing `#{ch}' in number"
	    else
	      non_digit = ch
	    end
	  else
	    non_digit = false
	    len0 = false
	  end
	else
	  ungetc
	  if len0
	    RubyLex.fail SyntaxError, "numeric literal without digits"
	  end
	  if non_digit
	    RubyLex.fail SyntaxError, "trailing `#{non_digit}' in number"
	  end
	  break
	end
      end
      return Token(TkINTEGER)
    end

    type = TkINTEGER
    allow_point = true
    allow_e = true
    non_digit = false
    while ch = getc
      case ch
      when /[0-9]/
	non_digit = false
      when "_"
	non_digit = ch
      when allow_point && "."
	if non_digit
	  RubyLex.fail SyntaxError, "trailing `#{non_digit}' in number"
	end
	type = TkFLOAT
	if peek(0) !~ /[0-9]/
	  type = TkINTEGER
	  ungetc
	  break
	end
	allow_point = false
      when allow_e && "e", allow_e && "E"
	if non_digit
	  RubyLex.fail SyntaxError, "trailing `#{non_digit}' in number"
	end
	type = TkFLOAT
	if peek(0) =~ /[+-]/
	  getc
	end
	allow_e = false
	allow_point = false
	non_digit = ch
      else
	if non_digit
	  RubyLex.fail SyntaxError, "trailing `#{non_digit}' in number"
	end
	ungetc
	break
      end
    end
    Token(type)
  end

  def identify_string(ltype, quoted = ltype)
    @ltype = ltype
    @quoted = quoted
    subtype = nil
    begin
      nest = 0
      while ch = getc
	if @quoted == ch and nest == 0
	  break
	elsif @ltype != "'" && ch == "#" && peek(0) == "{"
	  identify_string_dvar
	elsif @ltype != "'" && @ltype != "]" && @ltype != ":" and ch == "#"
	  subtype = true
	elsif ch == '\\' and @ltype == "'" #'
	  case ch = getc
	  when "\\", "\n", "'"
	  else
	    ungetc
	  end
	elsif ch == '\\' #'
	  read_escape
	end
	if PERCENT_PAREN.values.include?(@quoted)
	  if PERCENT_PAREN[ch] == @quoted
	    nest += 1
	  elsif ch == @quoted
	    nest -= 1
	  end
	end
      end
      if @ltype == "/"
        while /[imxoesun]/ =~ peek(0)
	  getc
	end
      end
      if subtype
	Token(DLtype2Token[ltype])
      else
	Token(Ltype2Token[ltype])
      end
    ensure
      @ltype = nil
      @quoted = nil
      @lex_state = EXPR_END
    end
  end

  def identify_string_dvar
    begin
      getc

      reserve_continue = @continue
      reserve_ltype = @ltype
      reserve_indent = @indent
      reserve_indent_stack = @indent_stack
      reserve_state = @lex_state
      reserve_quoted = @quoted

      @ltype = nil
      @quoted = nil
      @indent = 0
      @indent_stack = []
      @lex_state = EXPR_BEG

      loop do
	@continue = false
	prompt
	tk = token
	if @ltype or @continue or @indent > 0
	  next
	end
	break if tk.kind_of?(TkRBRACE)
      end
    ensure
      @continue = reserve_continue
      @ltype = reserve_ltype
      @indent = reserve_indent
      @indent_stack = reserve_indent_stack
      @lex_state = reserve_state
      @quoted = reserve_quoted
    end
  end

  def identify_comment
    @ltype = "#"

    while ch = getc
#      if ch == "\\" #"
#	read_escape
#      end
      if ch == "\n"
	@ltype = nil
	ungetc
	break
      end
    end
    return Token(TkCOMMENT)
  end

  def read_escape
    case ch = getc
    when "\n", "\r", "\f"
    when "\\", "n", "t", "r", "f", "v", "a", "e", "b", "s" #"
    when /[0-7]/
      ungetc ch
      3.times do
	case ch = getc
	when /[0-7]/
	when nil
	  break
	else
	  ungetc
	  break
	end
      end

    when "x"
      2.times do
	case ch = getc
	when /[0-9a-fA-F]/
	when nil
	  break
	else
	  ungetc
	  break
	end
      end

    when "M"
      if (ch = getc) != '-'
	ungetc
      else
	if (ch = getc) == "\\" #"
	  read_escape
	end
      end

    when "C", "c" #, "^"
      if ch == "C" and (ch = getc) != "-"
	ungetc
      elsif (ch = getc) == "\\" #"
	read_escape
      end
    else
      # other characters
    end
  end
end
