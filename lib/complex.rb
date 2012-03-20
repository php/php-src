# :enddoc:

warn('lib/complex.rb is deprecated') if $VERBOSE

require 'cmath'

unless defined?(Math.exp!)
  Object.instance_eval{remove_const :Math}
  Math = CMath
end

def Complex.generic? (other)
  other.kind_of?(Integer) ||
  other.kind_of?(Float)   ||
  other.kind_of?(Rational)
end

class Complex

  alias image imag

end

class Numeric

  def im() Complex(0, self) end

end
