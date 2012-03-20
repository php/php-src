##
# A TokenStream is a list of tokens, gathered during the parse of some entity
# (say a method). Entities populate these streams by being registered with the
# lexer. Any class can collect tokens by including TokenStream. From the
# outside, you use such an object by calling the start_collecting_tokens
# method, followed by calls to add_token and pop_token.

module RDoc::TokenStream

  ##
  # Adds +tokens+ to the collected tokens

  def add_tokens(*tokens)
    tokens.flatten.each { |token| @token_stream << token }
  end

  alias add_token add_tokens

  ##
  # Starts collecting tokens

  def collect_tokens
    @token_stream = []
  end

  alias start_collecting_tokens collect_tokens

  ##
  # Remove the last token from the collected tokens

  def pop_token
    @token_stream.pop
  end

  ##
  # Current token stream

  def token_stream
    @token_stream
  end

  ##
  # Returns a string representation of the token stream

  def tokens_to_s
    token_stream.map { |token| token.text }.join ''
  end

end

