class Integer < Numeric
  # call-seq:
  #     int.to_d  -> bigdecimal
  #
  # Convert +int+ to a BigDecimal and return it.
  #
  #     require 'bigdecimal'
  #     require 'bigdecimal/util'
  #
  #     42.to_d
  #     # => #<BigDecimal:1008ef070,'0.42E2',9(36)>
  #
  def to_d
    BigDecimal(self)
  end
end

class Float < Numeric
  # call-seq:
  #     flt.to_d  -> bigdecimal
  #
  # Convert +flt+ to a BigDecimal and return it.
  #
  #     require 'bigdecimal'
  #     require 'bigdecimal/util'
  #
  #     0.5.to_d
  #     # => #<BigDecimal:1dc69e0,'0.5E0',9(18)>
  #
  def to_d(precision=nil)
    BigDecimal(self, precision || Float::DIG+1)
  end
end

class String
  # call-seq:
  #     string.to_d  -> bigdecimal
  #
  # Convert +string+ to a BigDecimal and return it.
  #
  #     require 'bigdecimal'
  #     require 'bigdecimal/util'
  #
  #     "0.5".to_d
  #     # => #<BigDecimal:1dc69e0,'0.5E0',9(18)>
  #
  def to_d
    BigDecimal(self)
  end
end

class BigDecimal < Numeric
  # call-seq:
  #     a.to_digits -> string
  #
  # Converts a BigDecimal to a String of the form "nnnnnn.mmm".
  # This method is deprecated; use BigDecimal#to_s("F") instead.
  #
  #     require 'bigdecimal'
  #     require 'bigdecimal/util'
  #
  #     d = BigDecimal.new("3.14")
  #     d.to_digits
  #     # => "3.14"
  def to_digits
    if self.nan? || self.infinite? || self.zero?
      self.to_s
    else
      i       = self.to_i.to_s
      _,f,_,z = self.frac.split
      i + "." + ("0"*(-z)) + f
    end
  end

  # call-seq:
  #     a.to_d -> bigdecimal
  #
  # Returns self.
  def to_d
    self
  end
end

class Rational < Numeric
  # call-seq:
  #   r.to_d        -> bigdecimal
  #   r.to_d(sig)   -> bigdecimal
  #
  # Converts a Rational to a BigDecimal. Takes an optional parameter +sig+ to
  # limit the amount of significant digits.
  #
  #   r = (22/7.0).to_r
  #   # => (7077085128725065/2251799813685248)
  #   r.to_d
  #   # => #<BigDecimal:1a52bd8,'0.3142857142 8571427937 0154144999 105E1',45(63)>
  #   r.to_d(3)
  #   # => #<BigDecimal:1a44d08,'0.314E1',18(36)>
  def to_d(precision)
    if precision <= 0
      raise ArgumentError, "negative precision"
    end
    num = self.numerator
    BigDecimal(num).div(self.denominator, precision)
  end
end
