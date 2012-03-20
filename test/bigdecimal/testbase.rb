require "test/unit"
require "bigdecimal"

module TestBigDecimalBase
  def setup
    @mode = BigDecimal.mode(BigDecimal::EXCEPTION_ALL)
    BigDecimal.mode(BigDecimal::EXCEPTION_ALL, true)
    BigDecimal.mode(BigDecimal::EXCEPTION_UNDERFLOW, true)
    BigDecimal.mode(BigDecimal::EXCEPTION_OVERFLOW, true)
    BigDecimal.mode(BigDecimal::ROUND_MODE, BigDecimal::ROUND_HALF_UP)
    BigDecimal.limit(0)
  end

  def teardown
    [BigDecimal::EXCEPTION_INFINITY, BigDecimal::EXCEPTION_NaN,
     BigDecimal::EXCEPTION_UNDERFLOW, BigDecimal::EXCEPTION_OVERFLOW].each do |mode|
      BigDecimal.mode(mode, !(@mode & mode).zero?)
    end
  end

  def under_gc_stress
    stress, GC.stress = GC.stress, true
    yield
  ensure
    GC.stress = stress
  end
end
