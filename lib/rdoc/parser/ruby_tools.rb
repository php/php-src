##
# Collection of methods for writing parsers against RDoc::RubyLex and
# RDoc::RubyToken

module RDoc::Parser::RubyTools

  include RDoc::RubyToken

  ##
  # Adds a token listener +obj+, but you should probably use token_listener

  def add_token_listener(obj)
    @token_listeners ||= []
    @token_listeners << obj
  end

  ##
  # Fetches the next token from the scanner

  def get_tk
    tk = nil

    if @tokens.empty? then
      tk = @scanner.token
      @read.push @scanner.get_readed
      puts "get_tk1 => #{tk.inspect}" if $TOKEN_DEBUG
    else
      @read.push @unget_read.shift
      tk = @tokens.shift
      puts "get_tk2 => #{tk.inspect}" if $TOKEN_DEBUG
    end

    tk = nil if TkEND_OF_SCRIPT === tk

    if TkSYMBEG === tk then
      set_token_position tk.line_no, tk.char_no

      case tk1 = get_tk
      when TkId, TkOp, TkSTRING, TkDSTRING, TkSTAR, TkAMPER then
        if tk1.respond_to?(:name) then
          tk = Token(TkSYMBOL).set_text(":" + tk1.name)
        else
          tk = Token(TkSYMBOL).set_text(":" + tk1.text)
        end

        # remove the identifier we just read (we're about to replace it with a
        # symbol)
        @token_listeners.each do |obj|
          obj.pop_token
        end if @token_listeners
      else
        tk = tk1
      end
    end

    # inform any listeners of our shiny new token
    @token_listeners.each do |obj|
      obj.add_token(tk)
    end if @token_listeners

    tk
  end

  ##
  # Reads and returns all tokens up to one of +tokens+.  Leaves the matched
  # token in the token list.

  def get_tk_until(*tokens)
    read = []

    loop do
      tk = get_tk
      case tk when *tokens then unget_tk tk; break end
      read << tk
    end

    read
  end

  ##
  # Retrieves a String representation of the read tokens

  def get_tkread
    read = @read.join("")
    @read = []
    read
  end

  ##
  # Peek equivalent for get_tkread

  def peek_read
    @read.join('')
  end

  ##
  # Peek at the next token, but don't remove it from the stream

  def peek_tk
    unget_tk(tk = get_tk)
    tk
  end

  ##
  # Removes the token listener +obj+

  def remove_token_listener(obj)
    @token_listeners.delete(obj)
  end

  ##
  # Resets the tools

  def reset
    @read       = []
    @tokens     = []
    @unget_read = []
    @nest = 0
  end

  ##
  # Skips whitespace tokens including newlines if +skip_nl+ is true

  def skip_tkspace(skip_nl = true) # HACK dup
    tokens = []

    while TkSPACE === (tk = get_tk) or (skip_nl and TkNL === tk) do
      tokens.push tk
    end

    unget_tk tk
    tokens
  end

  ##
  # Has +obj+ listen to tokens

  def token_listener(obj)
    add_token_listener obj
    yield
  ensure
    remove_token_listener obj
  end

  ##
  # Returns +tk+ to the scanner

  def unget_tk(tk)
    @tokens.unshift tk
    @unget_read.unshift @read.pop

    # Remove this token from any listeners
    @token_listeners.each do |obj|
      obj.pop_token
    end if @token_listeners

    nil
  end

end


