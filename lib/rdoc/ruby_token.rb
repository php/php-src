#--
#   irb/ruby-token.rb - ruby tokens
#   	$Release Version: 0.9.5$
#   	$Revision: 11708 $
#   	$Date: 2007-02-12 15:01:19 -0800 (Mon, 12 Feb 2007) $
#   	by Keiju ISHITSUKA(keiju@ruby-lang.org)
#++
# Definitions of all tokens involved in the lexical analysis.
#
# This class is not documented because it is so deep in the internals.

module RDoc::RubyToken
  # :stopdoc:

  EXPR_BEG = :EXPR_BEG
  EXPR_MID = :EXPR_MID
  EXPR_END = :EXPR_END
  EXPR_ARG = :EXPR_ARG
  EXPR_FNAME = :EXPR_FNAME
  EXPR_DOT = :EXPR_DOT
  EXPR_CLASS = :EXPR_CLASS

  # for ruby 1.4X
  if !defined?(Symbol)
    Symbol = Integer
  end

  def set_token_position(line, char)
    @prev_line_no = line
    @prev_char_no = char
  end

  class Token
    def initialize(seek, line_no, char_no, text = nil)
      @seek = seek
      @line_no = line_no
      @char_no = char_no
      @text = text
    end

    attr :seek
    attr :line_no
    attr :char_no

    attr_accessor :text

    def ==(other)
      self.class == other.class and
      other.line_no == @line_no and
      other.char_no == @char_no and
      other.text == @text
    end

    ##
    # Because we're used in contexts that expect to return a token, we set the
    # text string and then return ourselves

    def set_text(text)
      @text = text
      self
    end

  end

  class TkNode < Token
    def initialize(seek, line_no, char_no, node = nil)
      super seek, line_no, char_no
      @node = node
    end

    attr :node

    def ==(other)
      self.class == other.class and
      other.line_no == @line_no and
      other.char_no == @char_no and
      other.node == @node
    end

    def set_text text
      @node = text
      self
    end

    alias text node
  end

  class TkId < Token
    def initialize(seek, line_no, char_no, name)
      super(seek, line_no, char_no)
      @name = name
    end
    attr :name

    def ==(other)
      self.class == other.class and
      other.line_no == @line_no and
      other.char_no == @char_no and
      other.name == @name
    end

    def set_text text
      @name = text
      self
    end

    alias text name
  end

  class TkKW < TkId
  end

  class TkVal < Token
    def initialize(seek, line_no, char_no, value = nil)
      super(seek, line_no, char_no)
      @value = value
    end
    attr_accessor :value

    def ==(other)
      self.class == other.class and
      other.line_no == @line_no and
      other.char_no == @char_no and
      other.value == @value
    end

    def set_text text
      @value = text
      self
    end

    alias text value
  end

  class TkOp < Token
    def initialize(seek, line_no, char_no, name = nil)
      super seek, line_no, char_no
      @name = name
    end

    attr_accessor :name

    def ==(other)
      self.class == other.class and
      other.line_no == @line_no and
      other.char_no == @char_no and
      other.name == @name
    end

    def set_text text
      @name = text
      self
    end

    alias text name
  end

  class TkOPASGN < TkOp
    def initialize(seek, line_no, char_no, op)
      super(seek, line_no, char_no)
      op = TkReading2Token[op][0] unless op.kind_of?(Symbol)
      @op = op
      @text = nil
    end

    attr :op

    def ==(other)
      self.class == other.class and
      other.line_no == @line_no and
      other.char_no == @char_no and
      other.op == @op
    end

    def text
      @text ||= "#{TkToken2Reading[op]}="
    end
  end

  class TkUnknownChar < Token
    def initialize(seek, line_no, char_no, name)
      super(seek, line_no, char_no)
      @name = name
    end
    attr :name

    def ==(other)
      self.class == other.class and
      other.line_no == @line_no and
      other.char_no == @char_no and
      other.name == @name
    end

    def set_text text
      @name = text
      self
    end

    alias text name
  end

  class TkError < Token
  end

  def Token(token, value = nil)
    value ||= TkToken2Reading[token]

    case token
    when String
      if (tk = TkReading2Token[token]).nil?
        IRB.fail TkReading2TokenNoKey, token
      end

      tk = Token(tk[0], value)

      if tk.kind_of?(TkOp) then
        tk.name = token
      end
    when Symbol
      if (tk = TkSymbol2Token[token]).nil?
        IRB.fail TkSymbol2TokenNoKey, token
      end

      tk = Token(tk[0], value)
    else
      if token.instance_method(:initialize).arity == 3 then
        tk = token.new(@prev_seek, @prev_line_no, @prev_char_no)
        tk.set_text value
      else
        tk = token.new(@prev_seek, @prev_line_no, @prev_char_no, value)
      end
    end

    tk
  end

  TokenDefinitions = [
    [:TkCLASS,      TkKW,  "class",  EXPR_CLASS],
    [:TkMODULE,     TkKW,  "module", EXPR_BEG],
    [:TkDEF,        TkKW,  "def",    EXPR_FNAME],
    [:TkUNDEF,      TkKW,  "undef",  EXPR_FNAME],
    [:TkBEGIN,      TkKW,  "begin",  EXPR_BEG],
    [:TkRESCUE,     TkKW,  "rescue", EXPR_MID],
    [:TkENSURE,     TkKW,  "ensure", EXPR_BEG],
    [:TkEND,        TkKW,  "end",    EXPR_END],
    [:TkIF,         TkKW,  "if",     EXPR_BEG, :TkIF_MOD],
    [:TkUNLESS,     TkKW,  "unless", EXPR_BEG, :TkUNLESS_MOD],
    [:TkTHEN,       TkKW,  "then",   EXPR_BEG],
    [:TkELSIF,      TkKW,  "elsif",  EXPR_BEG],
    [:TkELSE,       TkKW,  "else",   EXPR_BEG],
    [:TkCASE,       TkKW,  "case",   EXPR_BEG],
    [:TkWHEN,       TkKW,  "when",   EXPR_BEG],
    [:TkWHILE,      TkKW,  "while",  EXPR_BEG, :TkWHILE_MOD],
    [:TkUNTIL,      TkKW,  "until",  EXPR_BEG, :TkUNTIL_MOD],
    [:TkFOR,        TkKW,  "for",    EXPR_BEG],
    [:TkBREAK,      TkKW,  "break",  EXPR_MID],
    [:TkNEXT,       TkKW,  "next",   EXPR_END],
    [:TkREDO,       TkKW,  "redo",   EXPR_END],
    [:TkRETRY,      TkKW,  "retry",  EXPR_END],
    [:TkIN,         TkKW,  "in",     EXPR_BEG],
    [:TkDO,         TkKW,  "do",     EXPR_BEG],
    [:TkRETURN,     TkKW,  "return", EXPR_MID],
    [:TkYIELD,      TkKW,  "yield",  EXPR_END],
    [:TkSUPER,      TkKW,  "super",  EXPR_END],
    [:TkSELF,       TkKW,  "self",   EXPR_END],
    [:TkNIL,        TkKW,  "nil",    EXPR_END],
    [:TkTRUE,       TkKW,  "true",   EXPR_END],
    [:TkFALSE,      TkKW,  "false",  EXPR_END],
    [:TkAND,        TkKW,  "and",    EXPR_BEG],
    [:TkOR,         TkKW,  "or",     EXPR_BEG],
    [:TkNOT,        TkKW,  "not",    EXPR_BEG],
    [:TkIF_MOD,     TkKW],
    [:TkUNLESS_MOD, TkKW],
    [:TkWHILE_MOD,  TkKW],
    [:TkUNTIL_MOD,  TkKW],
    [:TkALIAS,      TkKW,  "alias",    EXPR_FNAME],
    [:TkDEFINED,    TkKW,  "defined?", EXPR_END],
    [:TklBEGIN,     TkKW,  "BEGIN",    EXPR_END],
    [:TklEND,       TkKW,  "END",      EXPR_END],
    [:Tk__LINE__,   TkKW,  "__LINE__", EXPR_END],
    [:Tk__FILE__,   TkKW,  "__FILE__", EXPR_END],

    [:TkIDENTIFIER, TkId],
    [:TkFID,        TkId],
    [:TkGVAR,       TkId],
    [:TkCVAR,       TkId],
    [:TkIVAR,       TkId],
    [:TkCONSTANT,   TkId],

    [:TkINTEGER,    TkVal],
    [:TkFLOAT,      TkVal],
    [:TkSTRING,     TkVal],
    [:TkXSTRING,    TkVal],
    [:TkREGEXP,     TkVal],
    [:TkSYMBOL,     TkVal],

    [:TkDSTRING,    TkNode],
    [:TkDXSTRING,   TkNode],
    [:TkDREGEXP,    TkNode],
    [:TkNTH_REF,    TkNode],
    [:TkBACK_REF,   TkNode],

    [:TkUPLUS,      TkOp,   "+@"],
    [:TkUMINUS,     TkOp,   "-@"],
    [:TkPOW,        TkOp,   "**"],
    [:TkCMP,        TkOp,   "<=>"],
    [:TkEQ,         TkOp,   "=="],
    [:TkEQQ,        TkOp,   "==="],
    [:TkNEQ,        TkOp,   "!="],
    [:TkGEQ,        TkOp,   ">="],
    [:TkLEQ,        TkOp,   "<="],
    [:TkANDOP,      TkOp,   "&&"],
    [:TkOROP,       TkOp,   "||"],
    [:TkMATCH,      TkOp,   "=~"],
    [:TkNMATCH,     TkOp,   "!~"],
    [:TkDOT2,       TkOp,   ".."],
    [:TkDOT3,       TkOp,   "..."],
    [:TkAREF,       TkOp,   "[]"],
    [:TkASET,       TkOp,   "[]="],
    [:TkLSHFT,      TkOp,   "<<"],
    [:TkRSHFT,      TkOp,   ">>"],
    [:TkCOLON2,     TkOp,   '::'],
    [:TkCOLON3,     TkOp,   '::'],
    #[:OPASGN,       TkOp],               # +=, -=  etc. #
    [:TkASSOC,      TkOp,   "=>"],
    [:TkQUESTION,   TkOp,   "?"], #?
    [:TkCOLON,      TkOp,   ":"],        #:

    [:TkfLPAREN,    Token,  "("], # func( #
    [:TkfLBRACK,    Token,  "["], # func[ #
    [:TkfLBRACE,    Token,  "{"], # func{ #
    [:TkSTAR,       Token,  "*"], # *arg
    [:TkAMPER,      Token,  "&"], # &arg #
    [:TkSYMBEG,     Token,  ":"], # :SYMBOL

    [:TkGT,         TkOp,   ">"],
    [:TkLT,         TkOp,   "<"],
    [:TkPLUS,       TkOp,   "+"],
    [:TkMINUS,      TkOp,   "-"],
    [:TkMULT,       TkOp,   "*"],
    [:TkDIV,        TkOp,   "/"],
    [:TkMOD,        TkOp,   "%"],
    [:TkBITOR,      TkOp,   "|"],
    [:TkBITXOR,     TkOp,   "^"],
    [:TkBITAND,     TkOp,   "&"],
    [:TkBITNOT,     TkOp,   "~"],
    [:TkNOTOP,      TkOp,   "!"],

    [:TkBACKQUOTE,  TkOp,   "`"],

    [:TkASSIGN,     Token,  "="],
    [:TkDOT,        Token,  "."],
    [:TkLPAREN,     Token,  "("],  #(exp)
    [:TkLBRACK,     Token,  "["],  #[arry]
    [:TkLBRACE,     Token,  "{"],  #{hash}
    [:TkRPAREN,     Token,  ")"],
    [:TkRBRACK,     Token,  "]"],
    [:TkRBRACE,     Token,  "}"],
    [:TkCOMMA,      Token,  ","],
    [:TkSEMICOLON,  Token,  ";"],

    [:TkCOMMENT,    TkVal],
    [:TkRD_COMMENT],
    [:TkSPACE,      Token,  " "],
    [:TkNL,         Token,  "\n"],
    [:TkEND_OF_SCRIPT],

    [:TkBACKSLASH,  TkUnknownChar,  "\\"],
    [:TkAT,         TkUnknownChar,  "@"],
    [:TkDOLLAR,     TkUnknownChar,  "$"],
  ]

  # {reading => token_class}
  # {reading => [token_class, *opt]}
  TkReading2Token = {}
  TkToken2Reading = {}
  TkSymbol2Token = {}

  def self.def_token(token_n, super_token = Token, reading = nil, *opts)
    token_n = token_n.id2name if token_n.kind_of?(Symbol)
    if const_defined?(token_n)
      IRB.fail AlreadyDefinedToken, token_n
    end
    token_c = eval("class #{token_n} < #{super_token}; end; #{token_n}")

    if reading
      TkToken2Reading[token_c] = reading

      return if TkReading2Token[reading]

      if opts.empty?
        TkReading2Token[reading] = [token_c]
      else
        TkReading2Token[reading] = [token_c].concat(opts)
      end
    end
    TkSymbol2Token[token_n.intern] = token_c
  end

  for defs in TokenDefinitions
    def_token(*defs)
  end

  NEWLINE_TOKEN = TkNL.new nil, 0, 0, "\n"

  class TkSYMBOL

    def to_sym
      @sym ||= text[1..-1].intern
    end

  end

  # :startdoc:
end

