require 'test/unit'

class ComplexRational_Test < Test::Unit::TestCase

  def test_rat_srat
    skip unless defined?(Rational)

    c = SimpleRat(1,3)
    cc = Rational(3,2)

    assert_kind_of(Numeric, c)
    assert_kind_of(Numeric, cc)

    assert_instance_of(SimpleRat, c)
    assert_instance_of(Rational, cc)

    assert_equal(SimpleRat(1,3), +c)
    assert_equal(SimpleRat(-1,3), -c)

    assert_equal(SimpleRat(7,3), c + 2)
    assert_equal(SimpleRat(-5,3), c - 2)
    assert_equal(SimpleRat(2,3), c * 2)
    assert_equal(SimpleRat(1,6), c / 2)
    assert_equal(SimpleRat(1,9), c ** 2)
    assert_equal(-1, c <=> 2)

    assert_equal(SimpleRat(7,3), 2 + c)
    assert_equal(SimpleRat(5,3), 2 - c)
    assert_equal(SimpleRat(2,3), 2 * c)
    assert_equal(SimpleRat(6,1), 2 / c)
    assert_in_delta(1.2599, 2 ** c, 0.001)
    assert_equal(1, 2 <=> c)

    assert_equal(SimpleRat(11,6), c + cc)
    assert_equal(SimpleRat(-7,6), c - cc)
    assert_equal(SimpleRat(1,2), c * cc)
    assert_equal(SimpleRat(2,9), c / cc)
    assert_in_delta(0.1924, c ** cc, 0.001)
    assert_equal(-1, c <=> cc)

    assert_equal(SimpleRat(11,6), cc + c)
    assert_equal(SimpleRat(7,6), cc - c)
    assert_equal(SimpleRat(1,2), cc * c)
    assert_equal(SimpleRat(9,2), cc / c)
    assert_in_delta(1.1447, cc ** c, 0.001)
    assert_equal(1, cc <=> c)

    assert_equal(SimpleRat, (+c).class)
    assert_equal(SimpleRat, (-c).class)

    assert_equal(SimpleRat, (c + 2).class)
    assert_equal(SimpleRat, (c - 2).class)
    assert_equal(SimpleRat, (c * 2).class)
    assert_equal(SimpleRat, (c / 2).class)
    assert_equal(SimpleRat, (c ** 2).class)

    assert_equal(SimpleRat, (2 + c).class)
    assert_equal(SimpleRat, (2 - c).class)
    assert_equal(SimpleRat, (2 * c).class)
    assert_equal(SimpleRat, (2 / c).class)
    assert_equal(Float, (2 ** c).class)

    assert_equal(SimpleRat, (c + cc).class)
    assert_equal(SimpleRat, (c - cc).class)
    assert_equal(SimpleRat, (c * cc).class)
    assert_equal(SimpleRat, (c / cc).class)
    assert_equal(Float, (c ** cc).class)

    assert_equal(SimpleRat, (cc + c).class)
    assert_equal(SimpleRat, (cc - c).class)
    assert_equal(SimpleRat, (cc * c).class)
    assert_equal(SimpleRat, (cc / c).class)
    assert_equal(Float, (cc ** c).class)

    assert_equal(0, Rational(2,3) <=> SimpleRat(2,3))
    assert_equal(0, SimpleRat(2,3) <=> Rational(2,3))
    assert(Rational(2,3) == SimpleRat(2,3))
    assert(SimpleRat(2,3) == Rational(2,3))

    assert_equal(SimpleRat, (c + 0).class)
    assert_equal(SimpleRat, (c - 0).class)
    assert_equal(SimpleRat, (c * 0).class)
    assert_equal(SimpleRat, (c * 1).class)
    assert_equal(SimpleRat, (0 + c).class)
    assert_equal(SimpleRat, (0 - c).class)
    assert_equal(SimpleRat, (0 * c).class)
    assert_equal(SimpleRat, (1 * c).class)
  end

  def test_comp_srat
    skip unless defined?(Rational)

    c = Complex(SimpleRat(2,3),SimpleRat(1,2))
    cc = Complex(Rational(3,2),Rational(2,1))

    assert_equal(Complex(SimpleRat(2,3),SimpleRat(1,2)), +c)
    assert_equal(Complex(SimpleRat(-2,3),SimpleRat(-1,2)), -c)

    assert_equal(Complex(SimpleRat(8,3),SimpleRat(1,2)), c + 2)
    assert_equal(Complex(SimpleRat(-4,3),SimpleRat(1,2)), c - 2)
    assert_equal(Complex(SimpleRat(4,3),SimpleRat(1,1)), c * 2)
    assert_equal(Complex(SimpleRat(1,3),SimpleRat(1,4)), c / 2)
    assert_equal(Complex(SimpleRat(7,36),SimpleRat(2,3)), c ** 2)
    assert_raise(NoMethodError){c <=> 2}

    assert_equal(Complex(SimpleRat(8,3),SimpleRat(1,2)), 2 + c)
    assert_equal(Complex(SimpleRat(4,3),SimpleRat(-1,2)), 2 - c)
    assert_equal(Complex(SimpleRat(4,3),SimpleRat(1,1)), 2 * c)
    assert_equal(Complex(SimpleRat(48,25),SimpleRat(-36,25)), 2 / c)
    r = 2 ** c
    assert_in_delta(1.4940, r.real, 0.001)
    assert_in_delta(0.5392, r.imag, 0.001)
    assert_raise(NoMethodError){2 <=> c}

    assert_equal(Complex(SimpleRat(13,6),SimpleRat(5,2)), c + cc)
    assert_equal(Complex(SimpleRat(-5,6),SimpleRat(-3,2)), c - cc)
    assert_equal(Complex(SimpleRat(0,1),SimpleRat(25,12)), c * cc)
    assert_equal(Complex(SimpleRat(8,25),SimpleRat(-7,75)), c / cc)
    r = c ** cc
    assert_in_delta(0.1732, r.real, 0.001)
    assert_in_delta(0.1186, r.imag, 0.001)
    assert_raise(NoMethodError){c <=> cc}

    assert_equal(Complex(SimpleRat(13,6),SimpleRat(5,2)), cc + c)
    assert_equal(Complex(SimpleRat(5,6),SimpleRat(3,2)), cc - c)
    assert_equal(Complex(SimpleRat(0,1),SimpleRat(25,12)), cc * c)
    assert_equal(Complex(SimpleRat(72,25),SimpleRat(21,25)), cc / c)
    r = cc ** c
    assert_in_delta(0.5498, r.real, 0.001)
    assert_in_delta(1.0198, r.imag, 0.001)
    assert_raise(NoMethodError){cc <=> c}

    assert_equal([SimpleRat,SimpleRat],
		 (+c).instance_eval{[real.class, imag.class]})
    assert_equal([SimpleRat,SimpleRat],
		 (-c).instance_eval{[real.class, imag.class]})

    assert_equal([SimpleRat,SimpleRat],
		 (c + 2).instance_eval{[real.class, imag.class]})
    assert_equal([SimpleRat,SimpleRat],
		 (c - 2).instance_eval{[real.class, imag.class]})
    assert_equal([SimpleRat,SimpleRat],
		 (c * 2).instance_eval{[real.class, imag.class]})
    assert_equal([SimpleRat,SimpleRat],
		 (c / 2).instance_eval{[real.class, imag.class]})
    assert_equal([SimpleRat,SimpleRat],
		 (c ** 2).instance_eval{[real.class, imag.class]})

    assert_equal([SimpleRat,SimpleRat],
		 (c + cc).instance_eval{[real.class, imag.class]})
    assert_equal([SimpleRat,SimpleRat],
		 (c - cc).instance_eval{[real.class, imag.class]})
    assert_equal([SimpleRat,SimpleRat],
		 (c * cc).instance_eval{[real.class, imag.class]})
    assert_equal([SimpleRat,SimpleRat],
		 (c / cc).instance_eval{[real.class, imag.class]})
    assert_equal([Float,Float],
		 (c ** cc).instance_eval{[real.class, imag.class]})

    assert_equal([SimpleRat,SimpleRat],
		 (cc + c).instance_eval{[real.class, imag.class]})
    assert_equal([SimpleRat,SimpleRat],
		 (cc - c).instance_eval{[real.class, imag.class]})
    assert_equal([SimpleRat,SimpleRat],
		 (cc * c).instance_eval{[real.class, imag.class]})
    assert_equal([SimpleRat,SimpleRat],
		 (cc / c).instance_eval{[real.class, imag.class]})
    assert_equal([Float,Float],
		 (cc ** c).instance_eval{[real.class, imag.class]})

    assert(Complex(SimpleRat(2,3),SimpleRat(3,2)) ==
	   Complex(Rational(2,3),Rational(3,2)))
    assert(Complex(Rational(2,3),Rational(3,2)) ==
	   Complex(SimpleRat(2,3),SimpleRat(3,2)))

    assert_equal([SimpleRat,SimpleRat],
		 (c + 0).instance_eval{[real.class, imag.class]})
    assert_equal([SimpleRat,SimpleRat],
		 (c - 0).instance_eval{[real.class, imag.class]})
    assert_equal([SimpleRat,SimpleRat],
		 (c * 0).instance_eval{[real.class, imag.class]})
    assert_equal([SimpleRat,SimpleRat],
		 (c * 1).instance_eval{[real.class, imag.class]})
    assert_equal([SimpleRat,SimpleRat],
		 (0 + c).instance_eval{[real.class, imag.class]})
    assert_equal([SimpleRat,SimpleRat],
		 (0 - c).instance_eval{[real.class, imag.class]})
    assert_equal([SimpleRat,SimpleRat],
		 (0 * c).instance_eval{[real.class, imag.class]})
    assert_equal([SimpleRat,SimpleRat],
		 (1 * c).instance_eval{[real.class, imag.class]})
  end

end

def SimpleRat(*a) SimpleRat.new(*a) end

class SimpleRat < Numeric

  def initialize(num, den = 1)
    if den == 0
      raise ZeroDivisionError, "divided by zero"
    end
    if den < 0
      num = -num
      den = -den
    end
    gcd = num.gcd(den)
    @num = num.div(gcd)
    @den = den.div(gcd)
  end

  def numerator() @num end
  def denominator() @den end

  def +@ () self end
  def -@ () self.class.new(-@num, @den) end

  def + (o)
    case o
    when SimpleRat, Rational
      a = @num * o.denominator
      b = o.numerator * @den
      self.class.new(a + b, @den * o.denominator)
    when Integer
      self + self.class.new(o)
    when Float
      to_f + o
    else
      x, y = o.coerce(self)
      x + y
    end
  end

  def - (o)
    case o
    when SimpleRat, Rational
      a = @num * o.denominator
      b = o.numerator * @den
      self.class.new(a - b, @den * o.denominator)
    when Integer
      self - self.class.new(o)
    when Float
      to_f - o
    else
      x, y = o.coerce(self)
      x - y
    end
  end

  def * (o)
    case o
    when SimpleRat, Rational
      a = @num * o.numerator
      b = @den * o.denominator
      self.class.new(a, b)
    when Integer
      self * self.class.new(o)
    when Float
      to_f * o
    else
      x, y = o.coerce(self)
      x * y
    end
  end

  def quo(o)
    case o
    when SimpleRat, Rational
      a = @num * o.denominator
      b = @den * o.numerator
      self.class.new(a, b)
    when Integer
      if o == 0
	raise raise ZeroDivisionError, "divided by zero"
      end
      self.quo(self.class.new(o))
    when Float
      to_f.quo(o)
    else
      x, y = o.coerce(self)
      x.quo(y)
    end
  end

  alias / quo

  def floor
    @num.div(@den)
  end

  def ceil
    -((-@num).div(@den))
  end

  def truncate
    if @num < 0
      return -((-@num).div(@den))
    end
    @num.div(@den)
  end

  alias to_i truncate

  def round
    if @num < 0
      num = -@num
      num = num * 2 + @den
      den = @den * 2
      -(num.div(den))
    else
      num = @num * 2 + @den
      den = @den * 2
      num.div(den)
    end
  end

  def div(o) (self / o).floor end
  def quot(o) (self / o).truncate end

  def modulo(o)
    q = div(o)
    self - o * q
  end

  def remainder(o)
    q = quot(o)
    self - o * q
  end

  alias % modulo

  def divmod(o) [div(o), modulo(o)] end
  def quotrem(o) [quot(o), remainder(o)] end

  def ** (o)
    case o
    when SimpleRat, Rational
      Float(self) ** o
    when Integer
      if o > 0
	a = @num ** o
	b = @den ** o
      elsif o < 0
	a = @den ** -o
	b = @num ** -o
      else
	a = b = 1
      end
      self.class.new(a, b)
    when Float
      to_f ** o
    else
      x, y = o.coerce(self)
      x ** y
    end
  end

  def <=> (o)
    case o
    when SimpleRat, Rational
      a = @num * o.denominator
      b = o.numerator * @den
      return a <=> b
    when Integer
      self <=> self.class.new(o)
    when Float
      to_f <=> o
    else
      x, y = o.coerce(self)
      x <=> y
    end
  end

  def == (o)
    begin
      (self <=> o) == 0
    rescue
      false
    end
  end

  def coerce(o)
    case o
    when Rational
      [self.class.new(o.numerator, o.denominator), self]
    when Integer
      [self.class.new(o), self]
    when Float
      [o, self.to_f]
    else
      super
    end
  end

  def hash() @num.hash ^ @den.hash end

  def to_f() @num.to_f / @den.to_f end
  def to_r() self end
  def to_s() format('%s/%s', @num, @den) end

  def inspect() format('#SR(%s)', to_s) end

  def marshal_dump() [@num, @den] end
  def marshal_load(a) @num, @den = a end

end
