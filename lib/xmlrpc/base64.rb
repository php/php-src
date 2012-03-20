=begin
= xmlrpc/base64.rb
Copyright (C) 2001, 2002, 2003 by Michael Neumann (mneumann@ntecs.de)

Released under the same term of license as Ruby.

= Classes
* ((<XMLRPC::Base64>))

= XMLRPC::Base64
== Description
This class is necessary for (('xmlrpc4r')) to determine that a string should
be transmitted base64-encoded and not as a raw-string.
You can use (({XMLRPC::Base64})) on the client and server-side as a
parameter and/or return-value.

== Class Methods
--- XMLRPC::Base64.new( str, state = :dec )
    Creates a new (({XMLRPC::Base64})) instance with string ((|str|)) as the
    internal string. When ((|state|)) is (({:dec})) it assumes that the
    string ((|str|)) is not in base64 format (perhaps already decoded),
    otherwise if ((|state|)) is (({:enc})) it decodes ((|str|))
    and stores it as the internal string.

--- XMLRPC::Base64.decode( str )
    Decodes string ((|str|)) with base64 and returns that value.

--- XMLRPC::Base64.encode( str )
    Encodes string ((|str|)) with base64 and returns that value.

== Instance Methods
--- XMLRPC::Base64#decoded
    Returns the internal string decoded.

--- XMLRPC::Base64#encoded
    Returns the internal string encoded with base64.

=end

module XMLRPC

class Base64

  def initialize(str, state = :dec)
    case state
    when :enc
      @str = Base64.decode(str)
    when :dec
      @str = str
    else
      raise ArgumentError, "wrong argument; either :enc or :dec"
    end
  end

  def decoded
    @str
  end

  def encoded
    Base64.encode(@str)
  end


  def Base64.decode(str)
    str.gsub(/\s+/, "").unpack("m")[0]
  end

  def Base64.encode(str)
    [str].pack("m")
  end

end


end # module XMLRPC


=begin
= History
    $Id$
=end
