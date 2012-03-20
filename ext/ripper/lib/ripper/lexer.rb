#
# $Id$
#
# Copyright (c) 2004,2005 Minero Aoki
#
# This program is free software.
# You can distribute and/or modify this program under the Ruby License.
# For details of Ruby License, see ruby/COPYING.
#

require 'ripper/core'

class Ripper

  # Tokenizes Ruby program and returns an Array of String.
  def Ripper.tokenize(src, filename = '-', lineno = 1)
    Lexer.new(src, filename, lineno).tokenize
  end

  # Tokenizes Ruby program and returns an Array of Array,
  # which is formatted like [[lineno, column], type, token].
  #
  #   require 'ripper'
  #   require 'pp'
  #
  #   p Ripper.lex("def m(a) nil end")
  #     #=> [[[1,  0], :on_kw,     "def"],
  #          [[1,  3], :on_sp,     " "  ],
  #          [[1,  4], :on_ident,  "m"  ],
  #          [[1,  5], :on_lparen, "("  ],
  #          [[1,  6], :on_ident,  "a"  ],
  #          [[1,  7], :on_rparen, ")"  ],
  #          [[1,  8], :on_sp,     " "  ],
  #          [[1,  9], :on_kw,     "nil"],
  #          [[1, 12], :on_sp,     " "  ],
  #          [[1, 13], :on_kw,     "end"]]
  #
  def Ripper.lex(src, filename = '-', lineno = 1)
    Lexer.new(src, filename, lineno).lex
  end

  class Lexer < ::Ripper   #:nodoc: internal use only
    def tokenize
      lex().map {|pos, event, tok| tok }
    end

    def lex
      parse().sort_by {|pos, event, tok| pos }
    end

    def parse
      @buf = []
      super
      @buf
    end

    private

    SCANNER_EVENTS.each do |event|
      module_eval(<<-End, __FILE__+'/module_eval', __LINE__ + 1)
        def on_#{event}(tok)
          @buf.push [[lineno(), column()], :on_#{event}, tok]
        end
      End
    end
  end

  # [EXPERIMENTAL]
  # Parses +src+ and return a string which was matched to +pattern+.
  # +pattern+ should be described as Regexp.
  #
  #   require 'ripper'
  #
  #   p Ripper.slice('def m(a) nil end', 'ident')                   #=> "m"
  #   p Ripper.slice('def m(a) nil end', '[ident lparen rparen]+')  #=> "m(a)"
  #   p Ripper.slice("<<EOS\nstring\nEOS",
  #                  'heredoc_beg nl $(tstring_content*) heredoc_end', 1)
  #       #=> "string\n"
  #
  def Ripper.slice(src, pattern, n = 0)
    if m = token_match(src, pattern)
    then m.string(n)
    else nil
    end
  end

  def Ripper.token_match(src, pattern)   #:nodoc:
    TokenPattern.compile(pattern).match(src)
  end

  class TokenPattern   #:nodoc:

    class Error < ::StandardError; end
    class CompileError < Error; end
    class MatchError < Error; end

    class << self
      alias compile new
    end

    def initialize(pattern)
      @source = pattern
      @re = compile(pattern)
    end

    def match(str)
      match_list(::Ripper.lex(str))
    end

    def match_list(tokens)
      if m = @re.match(map_tokens(tokens))
      then MatchData.new(tokens, m)
      else nil
      end
    end

    private

    def compile(pattern)
      if m = /[^\w\s$()\[\]{}?*+\.]/.match(pattern)
        raise CompileError, "invalid char in pattern: #{m[0].inspect}"
      end
      buf = ''
      pattern.scan(/(?:\w+|\$\(|[()\[\]\{\}?*+\.]+)/) do |tok|
        case tok
        when /\w/
          buf.concat map_token(tok)
        when '$('
          buf.concat '('
        when '('
          buf.concat '(?:'
        when /[?*\[\])\.]/
          buf.concat tok
        else
          raise 'must not happen'
        end
      end
      Regexp.compile(buf)
    rescue RegexpError => err
      raise CompileError, err.message
    end

    def map_tokens(tokens)
      tokens.map {|pos,type,str| map_token(type.to_s.sub(/\Aon_/,'')) }.join
    end

    MAP = {}
    seed = ('a'..'z').to_a + ('A'..'Z').to_a + ('0'..'9').to_a
    SCANNER_EVENT_TABLE.each do |ev, |
      raise CompileError, "[RIPPER FATAL] too many system token" if seed.empty?
      MAP[ev.to_s.sub(/\Aon_/,'')] = seed.shift
    end

    def map_token(tok)
      MAP[tok]  or raise CompileError, "unknown token: #{tok}"
    end

    class MatchData
      def initialize(tokens, match)
        @tokens = tokens
        @match = match
      end

      def string(n = 0)
        return nil unless @match
        match(n).join
      end

      private

      def match(n = 0)
        return [] unless @match
        @tokens[@match.begin(n)...@match.end(n)].map {|pos,type,str| str }
      end
    end

  end

end
