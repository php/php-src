require 'test/unit'

class Rational_Test2 < Test::Unit::TestCase

  def test_kumi
    assert_equal(Rational(1, 1), +Rational(1, 1))
    assert_equal(Rational(-1, 1), -Rational(1, 1))
    assert_equal(Rational(2, 1),
                 Rational(1, 1) + Rational(1, 1))
    assert_equal(Rational(0, 1),
                 Rational(1, 1) - Rational(1, 1))
    assert_equal(Rational(1, 1),
                 Rational(1, 1) * Rational(1, 1))
    assert_equal(Rational(1, 1),
                 Rational(1, 1) / Rational(1, 1))
    assert_equal(Rational(3, 1),
                 Rational(1, 1) + Rational(2, 1))
    assert_equal(Rational(-1, 1),
                 Rational(1, 1) - Rational(2, 1))
    assert_equal(Rational(2, 1),
                 Rational(1, 1) * Rational(2, 1))
    assert_equal(Rational(1, 2),
                 Rational(1, 1) / Rational(2, 1))
    assert_equal(Rational(4, 1),
                 Rational(1, 1) + Rational(3, 1))
    assert_equal(Rational(-2, 1),
                 Rational(1, 1) - Rational(3, 1))
    assert_equal(Rational(3, 1),
                 Rational(1, 1) * Rational(3, 1))
    assert_equal(Rational(1, 3),
                 Rational(1, 1) / Rational(3, 1))
    assert_equal(Rational(1073741790, 1),
                 Rational(1, 1) + Rational(1073741789, 1))
    assert_equal(Rational(-1073741788, 1),
                 Rational(1, 1) - Rational(1073741789, 1))
    assert_equal(Rational(1073741789, 1),
                 Rational(1, 1) * Rational(1073741789, 1))
    assert_equal(Rational(1, 1073741789),
                 Rational(1, 1) / Rational(1073741789, 1))
    assert_equal(Rational(1073741828, 1),
                 Rational(1, 1) + Rational(1073741827, 1))
    assert_equal(Rational(-1073741826, 1),
                 Rational(1, 1) - Rational(1073741827, 1))
    assert_equal(Rational(1073741827, 1),
                 Rational(1, 1) * Rational(1073741827, 1))
    assert_equal(Rational(1, 1073741827),
                 Rational(1, 1) / Rational(1073741827, 1))
    assert_equal(Rational(5, 3),
                 Rational(1, 1) + Rational(2, 3))
    assert_equal(Rational(1, 3),
                 Rational(1, 1) - Rational(2, 3))
    assert_equal(Rational(2, 3),
                 Rational(1, 1) * Rational(2, 3))
    assert_equal(Rational(3, 2),
                 Rational(1, 1) / Rational(2, 3))
    assert_equal(Rational(5, 2),
                 Rational(1, 1) + Rational(3, 2))
    assert_equal(Rational(-1, 2),
                 Rational(1, 1) - Rational(3, 2))
    assert_equal(Rational(3, 2),
                 Rational(1, 1) * Rational(3, 2))
    assert_equal(Rational(2, 3),
                 Rational(1, 1) / Rational(3, 2))
    assert_equal(Rational(1073741792, 1073741789),
                 Rational(1, 1) + Rational(3, 1073741789))
    assert_equal(Rational(1073741786, 1073741789),
                 Rational(1, 1) - Rational(3, 1073741789))
    assert_equal(Rational(3, 1073741789),
                 Rational(1, 1) * Rational(3, 1073741789))
    assert_equal(Rational(1073741789, 3),
                 Rational(1, 1) / Rational(3, 1073741789))
    assert_equal(Rational(1073741792, 3),
                 Rational(1, 1) + Rational(1073741789, 3))
    assert_equal(Rational(-1073741786, 3),
                 Rational(1, 1) - Rational(1073741789, 3))
    assert_equal(Rational(1073741789, 3),
                 Rational(1, 1) * Rational(1073741789, 3))
    assert_equal(Rational(3, 1073741789),
                 Rational(1, 1) / Rational(1073741789, 3))
    assert_equal(Rational(1073741830, 1073741827),
                 Rational(1, 1) + Rational(3, 1073741827))
    assert_equal(Rational(1073741824, 1073741827),
                 Rational(1, 1) - Rational(3, 1073741827))
    assert_equal(Rational(3, 1073741827),
                 Rational(1, 1) * Rational(3, 1073741827))
    assert_equal(Rational(1073741827, 3),
                 Rational(1, 1) / Rational(3, 1073741827))
    assert_equal(Rational(1073741830, 3),
                 Rational(1, 1) + Rational(1073741827, 3))
    assert_equal(Rational(-1073741824, 3),
                 Rational(1, 1) - Rational(1073741827, 3))
    assert_equal(Rational(1073741827, 3),
                 Rational(1, 1) * Rational(1073741827, 3))
    assert_equal(Rational(3, 1073741827),
                 Rational(1, 1) / Rational(1073741827, 3))
    assert_equal(Rational(2147483616, 1073741827),
                 Rational(1, 1) + Rational(1073741789, 1073741827))
    assert_equal(Rational(38, 1073741827),
                 Rational(1, 1) - Rational(1073741789, 1073741827))
    assert_equal(Rational(1073741789, 1073741827),
                 Rational(1, 1) * Rational(1073741789, 1073741827))
    assert_equal(Rational(1073741827, 1073741789),
                 Rational(1, 1) / Rational(1073741789, 1073741827))
    assert_equal(Rational(2147483616, 1073741789),
                 Rational(1, 1) + Rational(1073741827, 1073741789))
    assert_equal(Rational(-38, 1073741789),
                 Rational(1, 1) - Rational(1073741827, 1073741789))
    assert_equal(Rational(1073741827, 1073741789),
                 Rational(1, 1) * Rational(1073741827, 1073741789))
    assert_equal(Rational(1073741789, 1073741827),
                 Rational(1, 1) / Rational(1073741827, 1073741789))
    assert_equal(Rational(2, 1), +Rational(2, 1))
    assert_equal(Rational(-2, 1), -Rational(2, 1))
    assert_equal(Rational(3, 1),
                 Rational(2, 1) + Rational(1, 1))
    assert_equal(Rational(1, 1),
                 Rational(2, 1) - Rational(1, 1))
    assert_equal(Rational(2, 1),
                 Rational(2, 1) * Rational(1, 1))
    assert_equal(Rational(2, 1),
                 Rational(2, 1) / Rational(1, 1))
    assert_equal(Rational(4, 1),
                 Rational(2, 1) + Rational(2, 1))
    assert_equal(Rational(0, 1),
                 Rational(2, 1) - Rational(2, 1))
    assert_equal(Rational(4, 1),
                 Rational(2, 1) * Rational(2, 1))
    assert_equal(Rational(1, 1),
                 Rational(2, 1) / Rational(2, 1))
    assert_equal(Rational(5, 1),
                 Rational(2, 1) + Rational(3, 1))
    assert_equal(Rational(-1, 1),
                 Rational(2, 1) - Rational(3, 1))
    assert_equal(Rational(6, 1),
                 Rational(2, 1) * Rational(3, 1))
    assert_equal(Rational(2, 3),
                 Rational(2, 1) / Rational(3, 1))
    assert_equal(Rational(1073741791, 1),
                 Rational(2, 1) + Rational(1073741789, 1))
    assert_equal(Rational(-1073741787, 1),
                 Rational(2, 1) - Rational(1073741789, 1))
    assert_equal(Rational(2147483578, 1),
                 Rational(2, 1) * Rational(1073741789, 1))
    assert_equal(Rational(2, 1073741789),
                 Rational(2, 1) / Rational(1073741789, 1))
    assert_equal(Rational(1073741829, 1),
                 Rational(2, 1) + Rational(1073741827, 1))
    assert_equal(Rational(-1073741825, 1),
                 Rational(2, 1) - Rational(1073741827, 1))
    assert_equal(Rational(2147483654, 1),
                 Rational(2, 1) * Rational(1073741827, 1))
    assert_equal(Rational(2, 1073741827),
                 Rational(2, 1) / Rational(1073741827, 1))
    assert_equal(Rational(8, 3),
                 Rational(2, 1) + Rational(2, 3))
    assert_equal(Rational(4, 3),
                 Rational(2, 1) - Rational(2, 3))
    assert_equal(Rational(4, 3),
                 Rational(2, 1) * Rational(2, 3))
    assert_equal(Rational(3, 1),
                 Rational(2, 1) / Rational(2, 3))
    assert_equal(Rational(7, 2),
                 Rational(2, 1) + Rational(3, 2))
    assert_equal(Rational(1, 2),
                 Rational(2, 1) - Rational(3, 2))
    assert_equal(Rational(3, 1),
                 Rational(2, 1) * Rational(3, 2))
    assert_equal(Rational(4, 3),
                 Rational(2, 1) / Rational(3, 2))
    assert_equal(Rational(2147483581, 1073741789),
                 Rational(2, 1) + Rational(3, 1073741789))
    assert_equal(Rational(2147483575, 1073741789),
                 Rational(2, 1) - Rational(3, 1073741789))
    assert_equal(Rational(6, 1073741789),
                 Rational(2, 1) * Rational(3, 1073741789))
    assert_equal(Rational(2147483578, 3),
                 Rational(2, 1) / Rational(3, 1073741789))
    assert_equal(Rational(1073741795, 3),
                 Rational(2, 1) + Rational(1073741789, 3))
    assert_equal(Rational(-1073741783, 3),
                 Rational(2, 1) - Rational(1073741789, 3))
    assert_equal(Rational(2147483578, 3),
                 Rational(2, 1) * Rational(1073741789, 3))
    assert_equal(Rational(6, 1073741789),
                 Rational(2, 1) / Rational(1073741789, 3))
    assert_equal(Rational(2147483657, 1073741827),
                 Rational(2, 1) + Rational(3, 1073741827))
    assert_equal(Rational(2147483651, 1073741827),
                 Rational(2, 1) - Rational(3, 1073741827))
    assert_equal(Rational(6, 1073741827),
                 Rational(2, 1) * Rational(3, 1073741827))
    assert_equal(Rational(2147483654, 3),
                 Rational(2, 1) / Rational(3, 1073741827))
    assert_equal(Rational(1073741833, 3),
                 Rational(2, 1) + Rational(1073741827, 3))
    assert_equal(Rational(-1073741821, 3),
                 Rational(2, 1) - Rational(1073741827, 3))
    assert_equal(Rational(2147483654, 3),
                 Rational(2, 1) * Rational(1073741827, 3))
    assert_equal(Rational(6, 1073741827),
                 Rational(2, 1) / Rational(1073741827, 3))
    assert_equal(Rational(3221225443, 1073741827),
                 Rational(2, 1) + Rational(1073741789, 1073741827))
    assert_equal(Rational(1073741865, 1073741827),
                 Rational(2, 1) - Rational(1073741789, 1073741827))
    assert_equal(Rational(2147483578, 1073741827),
                 Rational(2, 1) * Rational(1073741789, 1073741827))
    assert_equal(Rational(2147483654, 1073741789),
                 Rational(2, 1) / Rational(1073741789, 1073741827))
    assert_equal(Rational(3221225405, 1073741789),
                 Rational(2, 1) + Rational(1073741827, 1073741789))
    assert_equal(Rational(1073741751, 1073741789),
                 Rational(2, 1) - Rational(1073741827, 1073741789))
    assert_equal(Rational(2147483654, 1073741789),
                 Rational(2, 1) * Rational(1073741827, 1073741789))
    assert_equal(Rational(2147483578, 1073741827),
                 Rational(2, 1) / Rational(1073741827, 1073741789))
    assert_equal(Rational(3, 1), +Rational(3, 1))
    assert_equal(Rational(-3, 1), -Rational(3, 1))
    assert_equal(Rational(4, 1),
                 Rational(3, 1) + Rational(1, 1))
    assert_equal(Rational(2, 1),
                 Rational(3, 1) - Rational(1, 1))
    assert_equal(Rational(3, 1),
                 Rational(3, 1) * Rational(1, 1))
    assert_equal(Rational(3, 1),
                 Rational(3, 1) / Rational(1, 1))
    assert_equal(Rational(5, 1),
                 Rational(3, 1) + Rational(2, 1))
    assert_equal(Rational(1, 1),
                 Rational(3, 1) - Rational(2, 1))
    assert_equal(Rational(6, 1),
                 Rational(3, 1) * Rational(2, 1))
    assert_equal(Rational(3, 2),
                 Rational(3, 1) / Rational(2, 1))
    assert_equal(Rational(6, 1),
                 Rational(3, 1) + Rational(3, 1))
    assert_equal(Rational(0, 1),
                 Rational(3, 1) - Rational(3, 1))
    assert_equal(Rational(9, 1),
                 Rational(3, 1) * Rational(3, 1))
    assert_equal(Rational(1, 1),
                 Rational(3, 1) / Rational(3, 1))
    assert_equal(Rational(1073741792, 1),
                 Rational(3, 1) + Rational(1073741789, 1))
    assert_equal(Rational(-1073741786, 1),
                 Rational(3, 1) - Rational(1073741789, 1))
    assert_equal(Rational(3221225367, 1),
                 Rational(3, 1) * Rational(1073741789, 1))
    assert_equal(Rational(3, 1073741789),
                 Rational(3, 1) / Rational(1073741789, 1))
    assert_equal(Rational(1073741830, 1),
                 Rational(3, 1) + Rational(1073741827, 1))
    assert_equal(Rational(-1073741824, 1),
                 Rational(3, 1) - Rational(1073741827, 1))
    assert_equal(Rational(3221225481, 1),
                 Rational(3, 1) * Rational(1073741827, 1))
    assert_equal(Rational(3, 1073741827),
                 Rational(3, 1) / Rational(1073741827, 1))
    assert_equal(Rational(11, 3),
                 Rational(3, 1) + Rational(2, 3))
    assert_equal(Rational(7, 3),
                 Rational(3, 1) - Rational(2, 3))
    assert_equal(Rational(2, 1),
                 Rational(3, 1) * Rational(2, 3))
    assert_equal(Rational(9, 2),
                 Rational(3, 1) / Rational(2, 3))
    assert_equal(Rational(9, 2),
                 Rational(3, 1) + Rational(3, 2))
    assert_equal(Rational(3, 2),
                 Rational(3, 1) - Rational(3, 2))
    assert_equal(Rational(9, 2),
                 Rational(3, 1) * Rational(3, 2))
    assert_equal(Rational(2, 1),
                 Rational(3, 1) / Rational(3, 2))
    assert_equal(Rational(3221225370, 1073741789),
                 Rational(3, 1) + Rational(3, 1073741789))
    assert_equal(Rational(3221225364, 1073741789),
                 Rational(3, 1) - Rational(3, 1073741789))
    assert_equal(Rational(9, 1073741789),
                 Rational(3, 1) * Rational(3, 1073741789))
    assert_equal(Rational(1073741789, 1),
                 Rational(3, 1) / Rational(3, 1073741789))
    assert_equal(Rational(1073741798, 3),
                 Rational(3, 1) + Rational(1073741789, 3))
    assert_equal(Rational(-1073741780, 3),
                 Rational(3, 1) - Rational(1073741789, 3))
    assert_equal(Rational(1073741789, 1),
                 Rational(3, 1) * Rational(1073741789, 3))
    assert_equal(Rational(9, 1073741789),
                 Rational(3, 1) / Rational(1073741789, 3))
    assert_equal(Rational(3221225484, 1073741827),
                 Rational(3, 1) + Rational(3, 1073741827))
    assert_equal(Rational(3221225478, 1073741827),
                 Rational(3, 1) - Rational(3, 1073741827))
    assert_equal(Rational(9, 1073741827),
                 Rational(3, 1) * Rational(3, 1073741827))
    assert_equal(Rational(1073741827, 1),
                 Rational(3, 1) / Rational(3, 1073741827))
    assert_equal(Rational(1073741836, 3),
                 Rational(3, 1) + Rational(1073741827, 3))
    assert_equal(Rational(-1073741818, 3),
                 Rational(3, 1) - Rational(1073741827, 3))
    assert_equal(Rational(1073741827, 1),
                 Rational(3, 1) * Rational(1073741827, 3))
    assert_equal(Rational(9, 1073741827),
                 Rational(3, 1) / Rational(1073741827, 3))
    assert_equal(Rational(4294967270, 1073741827),
                 Rational(3, 1) + Rational(1073741789, 1073741827))
    assert_equal(Rational(2147483692, 1073741827),
                 Rational(3, 1) - Rational(1073741789, 1073741827))
    assert_equal(Rational(3221225367, 1073741827),
                 Rational(3, 1) * Rational(1073741789, 1073741827))
    assert_equal(Rational(3221225481, 1073741789),
                 Rational(3, 1) / Rational(1073741789, 1073741827))
    assert_equal(Rational(4294967194, 1073741789),
                 Rational(3, 1) + Rational(1073741827, 1073741789))
    assert_equal(Rational(2147483540, 1073741789),
                 Rational(3, 1) - Rational(1073741827, 1073741789))
    assert_equal(Rational(3221225481, 1073741789),
                 Rational(3, 1) * Rational(1073741827, 1073741789))
    assert_equal(Rational(3221225367, 1073741827),
                 Rational(3, 1) / Rational(1073741827, 1073741789))
    assert_equal(Rational(1073741789, 1), +Rational(1073741789, 1))
    assert_equal(Rational(-1073741789, 1), -Rational(1073741789, 1))
    assert_equal(Rational(1073741790, 1),
                 Rational(1073741789, 1) + Rational(1, 1))
    assert_equal(Rational(1073741788, 1),
                 Rational(1073741789, 1) - Rational(1, 1))
    assert_equal(Rational(1073741789, 1),
                 Rational(1073741789, 1) * Rational(1, 1))
    assert_equal(Rational(1073741789, 1),
                 Rational(1073741789, 1) / Rational(1, 1))
    assert_equal(Rational(1073741791, 1),
                 Rational(1073741789, 1) + Rational(2, 1))
    assert_equal(Rational(1073741787, 1),
                 Rational(1073741789, 1) - Rational(2, 1))
    assert_equal(Rational(2147483578, 1),
                 Rational(1073741789, 1) * Rational(2, 1))
    assert_equal(Rational(1073741789, 2),
                 Rational(1073741789, 1) / Rational(2, 1))
    assert_equal(Rational(1073741792, 1),
                 Rational(1073741789, 1) + Rational(3, 1))
    assert_equal(Rational(1073741786, 1),
                 Rational(1073741789, 1) - Rational(3, 1))
    assert_equal(Rational(3221225367, 1),
                 Rational(1073741789, 1) * Rational(3, 1))
    assert_equal(Rational(1073741789, 3),
                 Rational(1073741789, 1) / Rational(3, 1))
    assert_equal(Rational(2147483578, 1),
                 Rational(1073741789, 1) + Rational(1073741789, 1))
    assert_equal(Rational(0, 1),
                 Rational(1073741789, 1) - Rational(1073741789, 1))
    assert_equal(Rational(1152921429444920521, 1),
                 Rational(1073741789, 1) * Rational(1073741789, 1))
    assert_equal(Rational(1, 1),
                 Rational(1073741789, 1) / Rational(1073741789, 1))
    assert_equal(Rational(2147483616, 1),
                 Rational(1073741789, 1) + Rational(1073741827, 1))
    assert_equal(Rational(-38, 1),
                 Rational(1073741789, 1) - Rational(1073741827, 1))
    assert_equal(Rational(1152921470247108503, 1),
                 Rational(1073741789, 1) * Rational(1073741827, 1))
    assert_equal(Rational(1073741789, 1073741827),
                 Rational(1073741789, 1) / Rational(1073741827, 1))
    assert_equal(Rational(3221225369, 3),
                 Rational(1073741789, 1) + Rational(2, 3))
    assert_equal(Rational(3221225365, 3),
                 Rational(1073741789, 1) - Rational(2, 3))
    assert_equal(Rational(2147483578, 3),
                 Rational(1073741789, 1) * Rational(2, 3))
    assert_equal(Rational(3221225367, 2),
                 Rational(1073741789, 1) / Rational(2, 3))
    assert_equal(Rational(2147483581, 2),
                 Rational(1073741789, 1) + Rational(3, 2))
    assert_equal(Rational(2147483575, 2),
                 Rational(1073741789, 1) - Rational(3, 2))
    assert_equal(Rational(3221225367, 2),
                 Rational(1073741789, 1) * Rational(3, 2))
    assert_equal(Rational(2147483578, 3),
                 Rational(1073741789, 1) / Rational(3, 2))
    assert_equal(Rational(1152921429444920524, 1073741789),
                 Rational(1073741789, 1) + Rational(3, 1073741789))
    assert_equal(Rational(1152921429444920518, 1073741789),
                 Rational(1073741789, 1) - Rational(3, 1073741789))
    assert_equal(Rational(3, 1),
                 Rational(1073741789, 1) * Rational(3, 1073741789))
    assert_equal(Rational(1152921429444920521, 3),
                 Rational(1073741789, 1) / Rational(3, 1073741789))
    assert_equal(Rational(4294967156, 3),
                 Rational(1073741789, 1) + Rational(1073741789, 3))
    assert_equal(Rational(2147483578, 3),
                 Rational(1073741789, 1) - Rational(1073741789, 3))
    assert_equal(Rational(1152921429444920521, 3),
                 Rational(1073741789, 1) * Rational(1073741789, 3))
    assert_equal(Rational(3, 1),
                 Rational(1073741789, 1) / Rational(1073741789, 3))
    assert_equal(Rational(1152921470247108506, 1073741827),
                 Rational(1073741789, 1) + Rational(3, 1073741827))
    assert_equal(Rational(1152921470247108500, 1073741827),
                 Rational(1073741789, 1) - Rational(3, 1073741827))
    assert_equal(Rational(3221225367, 1073741827),
                 Rational(1073741789, 1) * Rational(3, 1073741827))
    assert_equal(Rational(1152921470247108503, 3),
                 Rational(1073741789, 1) / Rational(3, 1073741827))
    assert_equal(Rational(4294967194, 3),
                 Rational(1073741789, 1) + Rational(1073741827, 3))
    assert_equal(Rational(2147483540, 3),
                 Rational(1073741789, 1) - Rational(1073741827, 3))
    assert_equal(Rational(1152921470247108503, 3),
                 Rational(1073741789, 1) * Rational(1073741827, 3))
    assert_equal(Rational(3221225367, 1073741827),
                 Rational(1073741789, 1) / Rational(1073741827, 3))
    assert_equal(Rational(1152921471320850292, 1073741827),
                 Rational(1073741789, 1) + Rational(1073741789, 1073741827))
    assert_equal(Rational(1152921469173366714, 1073741827),
                 Rational(1073741789, 1) - Rational(1073741789, 1073741827))
    assert_equal(Rational(1152921429444920521, 1073741827),
                 Rational(1073741789, 1) * Rational(1073741789, 1073741827))
    assert_equal(Rational(1073741827, 1),
                 Rational(1073741789, 1) / Rational(1073741789, 1073741827))
    assert_equal(Rational(1152921430518662348, 1073741789),
                 Rational(1073741789, 1) + Rational(1073741827, 1073741789))
    assert_equal(Rational(1152921428371178694, 1073741789),
                 Rational(1073741789, 1) - Rational(1073741827, 1073741789))
    assert_equal(Rational(1073741827, 1),
                 Rational(1073741789, 1) * Rational(1073741827, 1073741789))
    assert_equal(Rational(1152921429444920521, 1073741827),
                 Rational(1073741789, 1) / Rational(1073741827, 1073741789))
    assert_equal(Rational(1073741827, 1), +Rational(1073741827, 1))
    assert_equal(Rational(-1073741827, 1), -Rational(1073741827, 1))
    assert_equal(Rational(1073741828, 1),
                 Rational(1073741827, 1) + Rational(1, 1))
    assert_equal(Rational(1073741826, 1),
                 Rational(1073741827, 1) - Rational(1, 1))
    assert_equal(Rational(1073741827, 1),
                 Rational(1073741827, 1) * Rational(1, 1))
    assert_equal(Rational(1073741827, 1),
                 Rational(1073741827, 1) / Rational(1, 1))
    assert_equal(Rational(1073741829, 1),
                 Rational(1073741827, 1) + Rational(2, 1))
    assert_equal(Rational(1073741825, 1),
                 Rational(1073741827, 1) - Rational(2, 1))
    assert_equal(Rational(2147483654, 1),
                 Rational(1073741827, 1) * Rational(2, 1))
    assert_equal(Rational(1073741827, 2),
                 Rational(1073741827, 1) / Rational(2, 1))
    assert_equal(Rational(1073741830, 1),
                 Rational(1073741827, 1) + Rational(3, 1))
    assert_equal(Rational(1073741824, 1),
                 Rational(1073741827, 1) - Rational(3, 1))
    assert_equal(Rational(3221225481, 1),
                 Rational(1073741827, 1) * Rational(3, 1))
    assert_equal(Rational(1073741827, 3),
                 Rational(1073741827, 1) / Rational(3, 1))
    assert_equal(Rational(2147483616, 1),
                 Rational(1073741827, 1) + Rational(1073741789, 1))
    assert_equal(Rational(38, 1),
                 Rational(1073741827, 1) - Rational(1073741789, 1))
    assert_equal(Rational(1152921470247108503, 1),
                 Rational(1073741827, 1) * Rational(1073741789, 1))
    assert_equal(Rational(1073741827, 1073741789),
                 Rational(1073741827, 1) / Rational(1073741789, 1))
    assert_equal(Rational(2147483654, 1),
                 Rational(1073741827, 1) + Rational(1073741827, 1))
    assert_equal(Rational(0, 1),
                 Rational(1073741827, 1) - Rational(1073741827, 1))
    assert_equal(Rational(1152921511049297929, 1),
                 Rational(1073741827, 1) * Rational(1073741827, 1))
    assert_equal(Rational(1, 1),
                 Rational(1073741827, 1) / Rational(1073741827, 1))
    assert_equal(Rational(3221225483, 3),
                 Rational(1073741827, 1) + Rational(2, 3))
    assert_equal(Rational(3221225479, 3),
                 Rational(1073741827, 1) - Rational(2, 3))
    assert_equal(Rational(2147483654, 3),
                 Rational(1073741827, 1) * Rational(2, 3))
    assert_equal(Rational(3221225481, 2),
                 Rational(1073741827, 1) / Rational(2, 3))
    assert_equal(Rational(2147483657, 2),
                 Rational(1073741827, 1) + Rational(3, 2))
    assert_equal(Rational(2147483651, 2),
                 Rational(1073741827, 1) - Rational(3, 2))
    assert_equal(Rational(3221225481, 2),
                 Rational(1073741827, 1) * Rational(3, 2))
    assert_equal(Rational(2147483654, 3),
                 Rational(1073741827, 1) / Rational(3, 2))
    assert_equal(Rational(1152921470247108506, 1073741789),
                 Rational(1073741827, 1) + Rational(3, 1073741789))
    assert_equal(Rational(1152921470247108500, 1073741789),
                 Rational(1073741827, 1) - Rational(3, 1073741789))
    assert_equal(Rational(3221225481, 1073741789),
                 Rational(1073741827, 1) * Rational(3, 1073741789))
    assert_equal(Rational(1152921470247108503, 3),
                 Rational(1073741827, 1) / Rational(3, 1073741789))
    assert_equal(Rational(4294967270, 3),
                 Rational(1073741827, 1) + Rational(1073741789, 3))
    assert_equal(Rational(2147483692, 3),
                 Rational(1073741827, 1) - Rational(1073741789, 3))
    assert_equal(Rational(1152921470247108503, 3),
                 Rational(1073741827, 1) * Rational(1073741789, 3))
    assert_equal(Rational(3221225481, 1073741789),
                 Rational(1073741827, 1) / Rational(1073741789, 3))
    assert_equal(Rational(1152921511049297932, 1073741827),
                 Rational(1073741827, 1) + Rational(3, 1073741827))
    assert_equal(Rational(1152921511049297926, 1073741827),
                 Rational(1073741827, 1) - Rational(3, 1073741827))
    assert_equal(Rational(3, 1),
                 Rational(1073741827, 1) * Rational(3, 1073741827))
    assert_equal(Rational(1152921511049297929, 3),
                 Rational(1073741827, 1) / Rational(3, 1073741827))
    assert_equal(Rational(4294967308, 3),
                 Rational(1073741827, 1) + Rational(1073741827, 3))
    assert_equal(Rational(2147483654, 3),
                 Rational(1073741827, 1) - Rational(1073741827, 3))
    assert_equal(Rational(1152921511049297929, 3),
                 Rational(1073741827, 1) * Rational(1073741827, 3))
    assert_equal(Rational(3, 1),
                 Rational(1073741827, 1) / Rational(1073741827, 3))
    assert_equal(Rational(1152921512123039718, 1073741827),
                 Rational(1073741827, 1) + Rational(1073741789, 1073741827))
    assert_equal(Rational(1152921509975556140, 1073741827),
                 Rational(1073741827, 1) - Rational(1073741789, 1073741827))
    assert_equal(Rational(1073741789, 1),
                 Rational(1073741827, 1) * Rational(1073741789, 1073741827))
    assert_equal(Rational(1152921511049297929, 1073741789),
                 Rational(1073741827, 1) / Rational(1073741789, 1073741827))
    assert_equal(Rational(1152921471320850330, 1073741789),
                 Rational(1073741827, 1) + Rational(1073741827, 1073741789))
    assert_equal(Rational(1152921469173366676, 1073741789),
                 Rational(1073741827, 1) - Rational(1073741827, 1073741789))
    assert_equal(Rational(1152921511049297929, 1073741789),
                 Rational(1073741827, 1) * Rational(1073741827, 1073741789))
    assert_equal(Rational(1073741789, 1),
                 Rational(1073741827, 1) / Rational(1073741827, 1073741789))
    assert_equal(Rational(2, 3), +Rational(2, 3))
    assert_equal(Rational(-2, 3), -Rational(2, 3))
    assert_equal(Rational(5, 3),
                 Rational(2, 3) + Rational(1, 1))
    assert_equal(Rational(-1, 3),
                 Rational(2, 3) - Rational(1, 1))
    assert_equal(Rational(2, 3),
                 Rational(2, 3) * Rational(1, 1))
    assert_equal(Rational(2, 3),
                 Rational(2, 3) / Rational(1, 1))
    assert_equal(Rational(8, 3),
                 Rational(2, 3) + Rational(2, 1))
    assert_equal(Rational(-4, 3),
                 Rational(2, 3) - Rational(2, 1))
    assert_equal(Rational(4, 3),
                 Rational(2, 3) * Rational(2, 1))
    assert_equal(Rational(1, 3),
                 Rational(2, 3) / Rational(2, 1))
    assert_equal(Rational(11, 3),
                 Rational(2, 3) + Rational(3, 1))
    assert_equal(Rational(-7, 3),
                 Rational(2, 3) - Rational(3, 1))
    assert_equal(Rational(2, 1),
                 Rational(2, 3) * Rational(3, 1))
    assert_equal(Rational(2, 9),
                 Rational(2, 3) / Rational(3, 1))
    assert_equal(Rational(3221225369, 3),
                 Rational(2, 3) + Rational(1073741789, 1))
    assert_equal(Rational(-3221225365, 3),
                 Rational(2, 3) - Rational(1073741789, 1))
    assert_equal(Rational(2147483578, 3),
                 Rational(2, 3) * Rational(1073741789, 1))
    assert_equal(Rational(2, 3221225367),
                 Rational(2, 3) / Rational(1073741789, 1))
    assert_equal(Rational(3221225483, 3),
                 Rational(2, 3) + Rational(1073741827, 1))
    assert_equal(Rational(-3221225479, 3),
                 Rational(2, 3) - Rational(1073741827, 1))
    assert_equal(Rational(2147483654, 3),
                 Rational(2, 3) * Rational(1073741827, 1))
    assert_equal(Rational(2, 3221225481),
                 Rational(2, 3) / Rational(1073741827, 1))
    assert_equal(Rational(4, 3),
                 Rational(2, 3) + Rational(2, 3))
    assert_equal(Rational(0, 1),
                 Rational(2, 3) - Rational(2, 3))
    assert_equal(Rational(4, 9),
                 Rational(2, 3) * Rational(2, 3))
    assert_equal(Rational(1, 1),
                 Rational(2, 3) / Rational(2, 3))
    assert_equal(Rational(13, 6),
                 Rational(2, 3) + Rational(3, 2))
    assert_equal(Rational(-5, 6),
                 Rational(2, 3) - Rational(3, 2))
    assert_equal(Rational(1, 1),
                 Rational(2, 3) * Rational(3, 2))
    assert_equal(Rational(4, 9),
                 Rational(2, 3) / Rational(3, 2))
    assert_equal(Rational(2147483587, 3221225367),
                 Rational(2, 3) + Rational(3, 1073741789))
    assert_equal(Rational(2147483569, 3221225367),
                 Rational(2, 3) - Rational(3, 1073741789))
    assert_equal(Rational(2, 1073741789),
                 Rational(2, 3) * Rational(3, 1073741789))
    assert_equal(Rational(2147483578, 9),
                 Rational(2, 3) / Rational(3, 1073741789))
    assert_equal(Rational(1073741791, 3),
                 Rational(2, 3) + Rational(1073741789, 3))
    assert_equal(Rational(-357913929, 1),
                 Rational(2, 3) - Rational(1073741789, 3))
    assert_equal(Rational(2147483578, 9),
                 Rational(2, 3) * Rational(1073741789, 3))
    assert_equal(Rational(2, 1073741789),
                 Rational(2, 3) / Rational(1073741789, 3))
    assert_equal(Rational(2147483663, 3221225481),
                 Rational(2, 3) + Rational(3, 1073741827))
    assert_equal(Rational(2147483645, 3221225481),
                 Rational(2, 3) - Rational(3, 1073741827))
    assert_equal(Rational(2, 1073741827),
                 Rational(2, 3) * Rational(3, 1073741827))
    assert_equal(Rational(2147483654, 9),
                 Rational(2, 3) / Rational(3, 1073741827))
    assert_equal(Rational(357913943, 1),
                 Rational(2, 3) + Rational(1073741827, 3))
    assert_equal(Rational(-1073741825, 3),
                 Rational(2, 3) - Rational(1073741827, 3))
    assert_equal(Rational(2147483654, 9),
                 Rational(2, 3) * Rational(1073741827, 3))
    assert_equal(Rational(2, 1073741827),
                 Rational(2, 3) / Rational(1073741827, 3))
    assert_equal(Rational(5368709021, 3221225481),
                 Rational(2, 3) + Rational(1073741789, 1073741827))
    assert_equal(Rational(-1073741713, 3221225481),
                 Rational(2, 3) - Rational(1073741789, 1073741827))
    assert_equal(Rational(2147483578, 3221225481),
                 Rational(2, 3) * Rational(1073741789, 1073741827))
    assert_equal(Rational(2147483654, 3221225367),
                 Rational(2, 3) / Rational(1073741789, 1073741827))
    assert_equal(Rational(5368709059, 3221225367),
                 Rational(2, 3) + Rational(1073741827, 1073741789))
    assert_equal(Rational(-1073741903, 3221225367),
                 Rational(2, 3) - Rational(1073741827, 1073741789))
    assert_equal(Rational(2147483654, 3221225367),
                 Rational(2, 3) * Rational(1073741827, 1073741789))
    assert_equal(Rational(2147483578, 3221225481),
                 Rational(2, 3) / Rational(1073741827, 1073741789))
    assert_equal(Rational(3, 2), +Rational(3, 2))
    assert_equal(Rational(-3, 2), -Rational(3, 2))
    assert_equal(Rational(5, 2),
                 Rational(3, 2) + Rational(1, 1))
    assert_equal(Rational(1, 2),
                 Rational(3, 2) - Rational(1, 1))
    assert_equal(Rational(3, 2),
                 Rational(3, 2) * Rational(1, 1))
    assert_equal(Rational(3, 2),
                 Rational(3, 2) / Rational(1, 1))
    assert_equal(Rational(7, 2),
                 Rational(3, 2) + Rational(2, 1))
    assert_equal(Rational(-1, 2),
                 Rational(3, 2) - Rational(2, 1))
    assert_equal(Rational(3, 1),
                 Rational(3, 2) * Rational(2, 1))
    assert_equal(Rational(3, 4),
                 Rational(3, 2) / Rational(2, 1))
    assert_equal(Rational(9, 2),
                 Rational(3, 2) + Rational(3, 1))
    assert_equal(Rational(-3, 2),
                 Rational(3, 2) - Rational(3, 1))
    assert_equal(Rational(9, 2),
                 Rational(3, 2) * Rational(3, 1))
    assert_equal(Rational(1, 2),
                 Rational(3, 2) / Rational(3, 1))
    assert_equal(Rational(2147483581, 2),
                 Rational(3, 2) + Rational(1073741789, 1))
    assert_equal(Rational(-2147483575, 2),
                 Rational(3, 2) - Rational(1073741789, 1))
    assert_equal(Rational(3221225367, 2),
                 Rational(3, 2) * Rational(1073741789, 1))
    assert_equal(Rational(3, 2147483578),
                 Rational(3, 2) / Rational(1073741789, 1))
    assert_equal(Rational(2147483657, 2),
                 Rational(3, 2) + Rational(1073741827, 1))
    assert_equal(Rational(-2147483651, 2),
                 Rational(3, 2) - Rational(1073741827, 1))
    assert_equal(Rational(3221225481, 2),
                 Rational(3, 2) * Rational(1073741827, 1))
    assert_equal(Rational(3, 2147483654),
                 Rational(3, 2) / Rational(1073741827, 1))
    assert_equal(Rational(13, 6),
                 Rational(3, 2) + Rational(2, 3))
    assert_equal(Rational(5, 6),
                 Rational(3, 2) - Rational(2, 3))
    assert_equal(Rational(1, 1),
                 Rational(3, 2) * Rational(2, 3))
    assert_equal(Rational(9, 4),
                 Rational(3, 2) / Rational(2, 3))
    assert_equal(Rational(3, 1),
                 Rational(3, 2) + Rational(3, 2))
    assert_equal(Rational(0, 1),
                 Rational(3, 2) - Rational(3, 2))
    assert_equal(Rational(9, 4),
                 Rational(3, 2) * Rational(3, 2))
    assert_equal(Rational(1, 1),
                 Rational(3, 2) / Rational(3, 2))
    assert_equal(Rational(3221225373, 2147483578),
                 Rational(3, 2) + Rational(3, 1073741789))
    assert_equal(Rational(3221225361, 2147483578),
                 Rational(3, 2) - Rational(3, 1073741789))
    assert_equal(Rational(9, 2147483578),
                 Rational(3, 2) * Rational(3, 1073741789))
    assert_equal(Rational(1073741789, 2),
                 Rational(3, 2) / Rational(3, 1073741789))
    assert_equal(Rational(2147483587, 6),
                 Rational(3, 2) + Rational(1073741789, 3))
    assert_equal(Rational(-2147483569, 6),
                 Rational(3, 2) - Rational(1073741789, 3))
    assert_equal(Rational(1073741789, 2),
                 Rational(3, 2) * Rational(1073741789, 3))
    assert_equal(Rational(9, 2147483578),
                 Rational(3, 2) / Rational(1073741789, 3))
    assert_equal(Rational(3221225487, 2147483654),
                 Rational(3, 2) + Rational(3, 1073741827))
    assert_equal(Rational(3221225475, 2147483654),
                 Rational(3, 2) - Rational(3, 1073741827))
    assert_equal(Rational(9, 2147483654),
                 Rational(3, 2) * Rational(3, 1073741827))
    assert_equal(Rational(1073741827, 2),
                 Rational(3, 2) / Rational(3, 1073741827))
    assert_equal(Rational(2147483663, 6),
                 Rational(3, 2) + Rational(1073741827, 3))
    assert_equal(Rational(-2147483645, 6),
                 Rational(3, 2) - Rational(1073741827, 3))
    assert_equal(Rational(1073741827, 2),
                 Rational(3, 2) * Rational(1073741827, 3))
    assert_equal(Rational(9, 2147483654),
                 Rational(3, 2) / Rational(1073741827, 3))
    assert_equal(Rational(5368709059, 2147483654),
                 Rational(3, 2) + Rational(1073741789, 1073741827))
    assert_equal(Rational(1073741903, 2147483654),
                 Rational(3, 2) - Rational(1073741789, 1073741827))
    assert_equal(Rational(3221225367, 2147483654),
                 Rational(3, 2) * Rational(1073741789, 1073741827))
    assert_equal(Rational(3221225481, 2147483578),
                 Rational(3, 2) / Rational(1073741789, 1073741827))
    assert_equal(Rational(5368709021, 2147483578),
                 Rational(3, 2) + Rational(1073741827, 1073741789))
    assert_equal(Rational(1073741713, 2147483578),
                 Rational(3, 2) - Rational(1073741827, 1073741789))
    assert_equal(Rational(3221225481, 2147483578),
                 Rational(3, 2) * Rational(1073741827, 1073741789))
    assert_equal(Rational(3221225367, 2147483654),
                 Rational(3, 2) / Rational(1073741827, 1073741789))
    assert_equal(Rational(3, 1073741789), +Rational(3, 1073741789))
    assert_equal(Rational(-3, 1073741789), -Rational(3, 1073741789))
    assert_equal(Rational(1073741792, 1073741789),
                 Rational(3, 1073741789) + Rational(1, 1))
    assert_equal(Rational(-1073741786, 1073741789),
                 Rational(3, 1073741789) - Rational(1, 1))
    assert_equal(Rational(3, 1073741789),
                 Rational(3, 1073741789) * Rational(1, 1))
    assert_equal(Rational(3, 1073741789),
                 Rational(3, 1073741789) / Rational(1, 1))
    assert_equal(Rational(2147483581, 1073741789),
                 Rational(3, 1073741789) + Rational(2, 1))
    assert_equal(Rational(-2147483575, 1073741789),
                 Rational(3, 1073741789) - Rational(2, 1))
    assert_equal(Rational(6, 1073741789),
                 Rational(3, 1073741789) * Rational(2, 1))
    assert_equal(Rational(3, 2147483578),
                 Rational(3, 1073741789) / Rational(2, 1))
    assert_equal(Rational(3221225370, 1073741789),
                 Rational(3, 1073741789) + Rational(3, 1))
    assert_equal(Rational(-3221225364, 1073741789),
                 Rational(3, 1073741789) - Rational(3, 1))
    assert_equal(Rational(9, 1073741789),
                 Rational(3, 1073741789) * Rational(3, 1))
    assert_equal(Rational(1, 1073741789),
                 Rational(3, 1073741789) / Rational(3, 1))
    assert_equal(Rational(1152921429444920524, 1073741789),
                 Rational(3, 1073741789) + Rational(1073741789, 1))
    assert_equal(Rational(-1152921429444920518, 1073741789),
                 Rational(3, 1073741789) - Rational(1073741789, 1))
    assert_equal(Rational(3, 1),
                 Rational(3, 1073741789) * Rational(1073741789, 1))
    assert_equal(Rational(3, 1152921429444920521),
                 Rational(3, 1073741789) / Rational(1073741789, 1))
    assert_equal(Rational(1152921470247108506, 1073741789),
                 Rational(3, 1073741789) + Rational(1073741827, 1))
    assert_equal(Rational(-1152921470247108500, 1073741789),
                 Rational(3, 1073741789) - Rational(1073741827, 1))
    assert_equal(Rational(3221225481, 1073741789),
                 Rational(3, 1073741789) * Rational(1073741827, 1))
    assert_equal(Rational(3, 1152921470247108503),
                 Rational(3, 1073741789) / Rational(1073741827, 1))
    assert_equal(Rational(2147483587, 3221225367),
                 Rational(3, 1073741789) + Rational(2, 3))
    assert_equal(Rational(-2147483569, 3221225367),
                 Rational(3, 1073741789) - Rational(2, 3))
    assert_equal(Rational(2, 1073741789),
                 Rational(3, 1073741789) * Rational(2, 3))
    assert_equal(Rational(9, 2147483578),
                 Rational(3, 1073741789) / Rational(2, 3))
    assert_equal(Rational(3221225373, 2147483578),
                 Rational(3, 1073741789) + Rational(3, 2))
    assert_equal(Rational(-3221225361, 2147483578),
                 Rational(3, 1073741789) - Rational(3, 2))
    assert_equal(Rational(9, 2147483578),
                 Rational(3, 1073741789) * Rational(3, 2))
    assert_equal(Rational(2, 1073741789),
                 Rational(3, 1073741789) / Rational(3, 2))
    assert_equal(Rational(6, 1073741789),
                 Rational(3, 1073741789) + Rational(3, 1073741789))
    assert_equal(Rational(0, 1),
                 Rational(3, 1073741789) - Rational(3, 1073741789))
    assert_equal(Rational(9, 1152921429444920521),
                 Rational(3, 1073741789) * Rational(3, 1073741789))
    assert_equal(Rational(1, 1),
                 Rational(3, 1073741789) / Rational(3, 1073741789))
    assert_equal(Rational(1152921429444920530, 3221225367),
                 Rational(3, 1073741789) + Rational(1073741789, 3))
    assert_equal(Rational(-1152921429444920512, 3221225367),
                 Rational(3, 1073741789) - Rational(1073741789, 3))
    assert_equal(Rational(1, 1),
                 Rational(3, 1073741789) * Rational(1073741789, 3))
    assert_equal(Rational(9, 1152921429444920521),
                 Rational(3, 1073741789) / Rational(1073741789, 3))
    assert_equal(Rational(6442450848, 1152921470247108503),
                 Rational(3, 1073741789) + Rational(3, 1073741827))
    assert_equal(Rational(114, 1152921470247108503),
                 Rational(3, 1073741789) - Rational(3, 1073741827))
    assert_equal(Rational(9, 1152921470247108503),
                 Rational(3, 1073741789) * Rational(3, 1073741827))
    assert_equal(Rational(1073741827, 1073741789),
                 Rational(3, 1073741789) / Rational(3, 1073741827))
    assert_equal(Rational(1152921470247108512, 3221225367),
                 Rational(3, 1073741789) + Rational(1073741827, 3))
    assert_equal(Rational(-1152921470247108494, 3221225367),
                 Rational(3, 1073741789) - Rational(1073741827, 3))
    assert_equal(Rational(1073741827, 1073741789),
                 Rational(3, 1073741789) * Rational(1073741827, 3))
    assert_equal(Rational(9, 1152921470247108503),
                 Rational(3, 1073741789) / Rational(1073741827, 3))
    assert_equal(Rational(1152921432666146002, 1152921470247108503),
                 Rational(3, 1073741789) + Rational(1073741789, 1073741827))
    assert_equal(Rational(-1152921426223695040, 1152921470247108503),
                 Rational(3, 1073741789) - Rational(1073741789, 1073741827))
    assert_equal(Rational(3, 1073741827),
                 Rational(3, 1073741789) * Rational(1073741789, 1073741827))
    assert_equal(Rational(3221225481, 1152921429444920521),
                 Rational(3, 1073741789) / Rational(1073741789, 1073741827))
    assert_equal(Rational(1073741830, 1073741789),
                 Rational(3, 1073741789) + Rational(1073741827, 1073741789))
    assert_equal(Rational(-1073741824, 1073741789),
                 Rational(3, 1073741789) - Rational(1073741827, 1073741789))
    assert_equal(Rational(3221225481, 1152921429444920521),
                 Rational(3, 1073741789) * Rational(1073741827, 1073741789))
    assert_equal(Rational(3, 1073741827),
                 Rational(3, 1073741789) / Rational(1073741827, 1073741789))
    assert_equal(Rational(1073741789, 3), +Rational(1073741789, 3))
    assert_equal(Rational(-1073741789, 3), -Rational(1073741789, 3))
    assert_equal(Rational(1073741792, 3),
                 Rational(1073741789, 3) + Rational(1, 1))
    assert_equal(Rational(1073741786, 3),
                 Rational(1073741789, 3) - Rational(1, 1))
    assert_equal(Rational(1073741789, 3),
                 Rational(1073741789, 3) * Rational(1, 1))
    assert_equal(Rational(1073741789, 3),
                 Rational(1073741789, 3) / Rational(1, 1))
    assert_equal(Rational(1073741795, 3),
                 Rational(1073741789, 3) + Rational(2, 1))
    assert_equal(Rational(1073741783, 3),
                 Rational(1073741789, 3) - Rational(2, 1))
    assert_equal(Rational(2147483578, 3),
                 Rational(1073741789, 3) * Rational(2, 1))
    assert_equal(Rational(1073741789, 6),
                 Rational(1073741789, 3) / Rational(2, 1))
    assert_equal(Rational(1073741798, 3),
                 Rational(1073741789, 3) + Rational(3, 1))
    assert_equal(Rational(1073741780, 3),
                 Rational(1073741789, 3) - Rational(3, 1))
    assert_equal(Rational(1073741789, 1),
                 Rational(1073741789, 3) * Rational(3, 1))
    assert_equal(Rational(1073741789, 9),
                 Rational(1073741789, 3) / Rational(3, 1))
    assert_equal(Rational(4294967156, 3),
                 Rational(1073741789, 3) + Rational(1073741789, 1))
    assert_equal(Rational(-2147483578, 3),
                 Rational(1073741789, 3) - Rational(1073741789, 1))
    assert_equal(Rational(1152921429444920521, 3),
                 Rational(1073741789, 3) * Rational(1073741789, 1))
    assert_equal(Rational(1, 3),
                 Rational(1073741789, 3) / Rational(1073741789, 1))
    assert_equal(Rational(4294967270, 3),
                 Rational(1073741789, 3) + Rational(1073741827, 1))
    assert_equal(Rational(-2147483692, 3),
                 Rational(1073741789, 3) - Rational(1073741827, 1))
    assert_equal(Rational(1152921470247108503, 3),
                 Rational(1073741789, 3) * Rational(1073741827, 1))
    assert_equal(Rational(1073741789, 3221225481),
                 Rational(1073741789, 3) / Rational(1073741827, 1))
    assert_equal(Rational(1073741791, 3),
                 Rational(1073741789, 3) + Rational(2, 3))
    assert_equal(Rational(357913929, 1),
                 Rational(1073741789, 3) - Rational(2, 3))
    assert_equal(Rational(2147483578, 9),
                 Rational(1073741789, 3) * Rational(2, 3))
    assert_equal(Rational(1073741789, 2),
                 Rational(1073741789, 3) / Rational(2, 3))
    assert_equal(Rational(2147483587, 6),
                 Rational(1073741789, 3) + Rational(3, 2))
    assert_equal(Rational(2147483569, 6),
                 Rational(1073741789, 3) - Rational(3, 2))
    assert_equal(Rational(1073741789, 2),
                 Rational(1073741789, 3) * Rational(3, 2))
    assert_equal(Rational(2147483578, 9),
                 Rational(1073741789, 3) / Rational(3, 2))
    assert_equal(Rational(1152921429444920530, 3221225367),
                 Rational(1073741789, 3) + Rational(3, 1073741789))
    assert_equal(Rational(1152921429444920512, 3221225367),
                 Rational(1073741789, 3) - Rational(3, 1073741789))
    assert_equal(Rational(1, 1),
                 Rational(1073741789, 3) * Rational(3, 1073741789))
    assert_equal(Rational(1152921429444920521, 9),
                 Rational(1073741789, 3) / Rational(3, 1073741789))
    assert_equal(Rational(2147483578, 3),
                 Rational(1073741789, 3) + Rational(1073741789, 3))
    assert_equal(Rational(0, 1),
                 Rational(1073741789, 3) - Rational(1073741789, 3))
    assert_equal(Rational(1152921429444920521, 9),
                 Rational(1073741789, 3) * Rational(1073741789, 3))
    assert_equal(Rational(1, 1),
                 Rational(1073741789, 3) / Rational(1073741789, 3))
    assert_equal(Rational(1152921470247108512, 3221225481),
                 Rational(1073741789, 3) + Rational(3, 1073741827))
    assert_equal(Rational(1152921470247108494, 3221225481),
                 Rational(1073741789, 3) - Rational(3, 1073741827))
    assert_equal(Rational(1073741789, 1073741827),
                 Rational(1073741789, 3) * Rational(3, 1073741827))
    assert_equal(Rational(1152921470247108503, 9),
                 Rational(1073741789, 3) / Rational(3, 1073741827))
    assert_equal(Rational(715827872, 1),
                 Rational(1073741789, 3) + Rational(1073741827, 3))
    assert_equal(Rational(-38, 3),
                 Rational(1073741789, 3) - Rational(1073741827, 3))
    assert_equal(Rational(1152921470247108503, 9),
                 Rational(1073741789, 3) * Rational(1073741827, 3))
    assert_equal(Rational(1073741789, 1073741827),
                 Rational(1073741789, 3) / Rational(1073741827, 3))
    assert_equal(Rational(1152921473468333870, 3221225481),
                 Rational(1073741789, 3) + Rational(1073741789, 1073741827))
    assert_equal(Rational(1152921467025883136, 3221225481),
                 Rational(1073741789, 3) - Rational(1073741789, 1073741827))
    assert_equal(Rational(1152921429444920521, 3221225481),
                 Rational(1073741789, 3) * Rational(1073741789, 1073741827))
    assert_equal(Rational(1073741827, 3),
                 Rational(1073741789, 3) / Rational(1073741789, 1073741827))
    assert_equal(Rational(1152921432666146002, 3221225367),
                 Rational(1073741789, 3) + Rational(1073741827, 1073741789))
    assert_equal(Rational(1152921426223695040, 3221225367),
                 Rational(1073741789, 3) - Rational(1073741827, 1073741789))
    assert_equal(Rational(1073741827, 3),
                 Rational(1073741789, 3) * Rational(1073741827, 1073741789))
    assert_equal(Rational(1152921429444920521, 3221225481),
                 Rational(1073741789, 3) / Rational(1073741827, 1073741789))
    assert_equal(Rational(3, 1073741827), +Rational(3, 1073741827))
    assert_equal(Rational(-3, 1073741827), -Rational(3, 1073741827))
    assert_equal(Rational(1073741830, 1073741827),
                 Rational(3, 1073741827) + Rational(1, 1))
    assert_equal(Rational(-1073741824, 1073741827),
                 Rational(3, 1073741827) - Rational(1, 1))
    assert_equal(Rational(3, 1073741827),
                 Rational(3, 1073741827) * Rational(1, 1))
    assert_equal(Rational(3, 1073741827),
                 Rational(3, 1073741827) / Rational(1, 1))
    assert_equal(Rational(2147483657, 1073741827),
                 Rational(3, 1073741827) + Rational(2, 1))
    assert_equal(Rational(-2147483651, 1073741827),
                 Rational(3, 1073741827) - Rational(2, 1))
    assert_equal(Rational(6, 1073741827),
                 Rational(3, 1073741827) * Rational(2, 1))
    assert_equal(Rational(3, 2147483654),
                 Rational(3, 1073741827) / Rational(2, 1))
    assert_equal(Rational(3221225484, 1073741827),
                 Rational(3, 1073741827) + Rational(3, 1))
    assert_equal(Rational(-3221225478, 1073741827),
                 Rational(3, 1073741827) - Rational(3, 1))
    assert_equal(Rational(9, 1073741827),
                 Rational(3, 1073741827) * Rational(3, 1))
    assert_equal(Rational(1, 1073741827),
                 Rational(3, 1073741827) / Rational(3, 1))
    assert_equal(Rational(1152921470247108506, 1073741827),
                 Rational(3, 1073741827) + Rational(1073741789, 1))
    assert_equal(Rational(-1152921470247108500, 1073741827),
                 Rational(3, 1073741827) - Rational(1073741789, 1))
    assert_equal(Rational(3221225367, 1073741827),
                 Rational(3, 1073741827) * Rational(1073741789, 1))
    assert_equal(Rational(3, 1152921470247108503),
                 Rational(3, 1073741827) / Rational(1073741789, 1))
    assert_equal(Rational(1152921511049297932, 1073741827),
                 Rational(3, 1073741827) + Rational(1073741827, 1))
    assert_equal(Rational(-1152921511049297926, 1073741827),
                 Rational(3, 1073741827) - Rational(1073741827, 1))
    assert_equal(Rational(3, 1),
                 Rational(3, 1073741827) * Rational(1073741827, 1))
    assert_equal(Rational(3, 1152921511049297929),
                 Rational(3, 1073741827) / Rational(1073741827, 1))
    assert_equal(Rational(2147483663, 3221225481),
                 Rational(3, 1073741827) + Rational(2, 3))
    assert_equal(Rational(-2147483645, 3221225481),
                 Rational(3, 1073741827) - Rational(2, 3))
    assert_equal(Rational(2, 1073741827),
                 Rational(3, 1073741827) * Rational(2, 3))
    assert_equal(Rational(9, 2147483654),
                 Rational(3, 1073741827) / Rational(2, 3))
    assert_equal(Rational(3221225487, 2147483654),
                 Rational(3, 1073741827) + Rational(3, 2))
    assert_equal(Rational(-3221225475, 2147483654),
                 Rational(3, 1073741827) - Rational(3, 2))
    assert_equal(Rational(9, 2147483654),
                 Rational(3, 1073741827) * Rational(3, 2))
    assert_equal(Rational(2, 1073741827),
                 Rational(3, 1073741827) / Rational(3, 2))
    assert_equal(Rational(6442450848, 1152921470247108503),
                 Rational(3, 1073741827) + Rational(3, 1073741789))
    assert_equal(Rational(-114, 1152921470247108503),
                 Rational(3, 1073741827) - Rational(3, 1073741789))
    assert_equal(Rational(9, 1152921470247108503),
                 Rational(3, 1073741827) * Rational(3, 1073741789))
    assert_equal(Rational(1073741789, 1073741827),
                 Rational(3, 1073741827) / Rational(3, 1073741789))
    assert_equal(Rational(1152921470247108512, 3221225481),
                 Rational(3, 1073741827) + Rational(1073741789, 3))
    assert_equal(Rational(-1152921470247108494, 3221225481),
                 Rational(3, 1073741827) - Rational(1073741789, 3))
    assert_equal(Rational(1073741789, 1073741827),
                 Rational(3, 1073741827) * Rational(1073741789, 3))
    assert_equal(Rational(9, 1152921470247108503),
                 Rational(3, 1073741827) / Rational(1073741789, 3))
    assert_equal(Rational(6, 1073741827),
                 Rational(3, 1073741827) + Rational(3, 1073741827))
    assert_equal(Rational(0, 1),
                 Rational(3, 1073741827) - Rational(3, 1073741827))
    assert_equal(Rational(9, 1152921511049297929),
                 Rational(3, 1073741827) * Rational(3, 1073741827))
    assert_equal(Rational(1, 1),
                 Rational(3, 1073741827) / Rational(3, 1073741827))
    assert_equal(Rational(1152921511049297938, 3221225481),
                 Rational(3, 1073741827) + Rational(1073741827, 3))
    assert_equal(Rational(-1152921511049297920, 3221225481),
                 Rational(3, 1073741827) - Rational(1073741827, 3))
    assert_equal(Rational(1, 1),
                 Rational(3, 1073741827) * Rational(1073741827, 3))
    assert_equal(Rational(9, 1152921511049297929),
                 Rational(3, 1073741827) / Rational(1073741827, 3))
    assert_equal(Rational(1073741792, 1073741827),
                 Rational(3, 1073741827) + Rational(1073741789, 1073741827))
    assert_equal(Rational(-1073741786, 1073741827),
                 Rational(3, 1073741827) - Rational(1073741789, 1073741827))
    assert_equal(Rational(3221225367, 1152921511049297929),
                 Rational(3, 1073741827) * Rational(1073741789, 1073741827))
    assert_equal(Rational(3, 1073741789),
                 Rational(3, 1073741827) / Rational(1073741789, 1073741827))
    assert_equal(Rational(1152921514270523296, 1152921470247108503),
                 Rational(3, 1073741827) + Rational(1073741827, 1073741789))
    assert_equal(Rational(-1152921507828072562, 1152921470247108503),
                 Rational(3, 1073741827) - Rational(1073741827, 1073741789))
    assert_equal(Rational(3, 1073741789),
                 Rational(3, 1073741827) * Rational(1073741827, 1073741789))
    assert_equal(Rational(3221225367, 1152921511049297929),
                 Rational(3, 1073741827) / Rational(1073741827, 1073741789))
    assert_equal(Rational(1073741827, 3), +Rational(1073741827, 3))
    assert_equal(Rational(-1073741827, 3), -Rational(1073741827, 3))
    assert_equal(Rational(1073741830, 3),
                 Rational(1073741827, 3) + Rational(1, 1))
    assert_equal(Rational(1073741824, 3),
                 Rational(1073741827, 3) - Rational(1, 1))
    assert_equal(Rational(1073741827, 3),
                 Rational(1073741827, 3) * Rational(1, 1))
    assert_equal(Rational(1073741827, 3),
                 Rational(1073741827, 3) / Rational(1, 1))
    assert_equal(Rational(1073741833, 3),
                 Rational(1073741827, 3) + Rational(2, 1))
    assert_equal(Rational(1073741821, 3),
                 Rational(1073741827, 3) - Rational(2, 1))
    assert_equal(Rational(2147483654, 3),
                 Rational(1073741827, 3) * Rational(2, 1))
    assert_equal(Rational(1073741827, 6),
                 Rational(1073741827, 3) / Rational(2, 1))
    assert_equal(Rational(1073741836, 3),
                 Rational(1073741827, 3) + Rational(3, 1))
    assert_equal(Rational(1073741818, 3),
                 Rational(1073741827, 3) - Rational(3, 1))
    assert_equal(Rational(1073741827, 1),
                 Rational(1073741827, 3) * Rational(3, 1))
    assert_equal(Rational(1073741827, 9),
                 Rational(1073741827, 3) / Rational(3, 1))
    assert_equal(Rational(4294967194, 3),
                 Rational(1073741827, 3) + Rational(1073741789, 1))
    assert_equal(Rational(-2147483540, 3),
                 Rational(1073741827, 3) - Rational(1073741789, 1))
    assert_equal(Rational(1152921470247108503, 3),
                 Rational(1073741827, 3) * Rational(1073741789, 1))
    assert_equal(Rational(1073741827, 3221225367),
                 Rational(1073741827, 3) / Rational(1073741789, 1))
    assert_equal(Rational(4294967308, 3),
                 Rational(1073741827, 3) + Rational(1073741827, 1))
    assert_equal(Rational(-2147483654, 3),
                 Rational(1073741827, 3) - Rational(1073741827, 1))
    assert_equal(Rational(1152921511049297929, 3),
                 Rational(1073741827, 3) * Rational(1073741827, 1))
    assert_equal(Rational(1, 3),
                 Rational(1073741827, 3) / Rational(1073741827, 1))
    assert_equal(Rational(357913943, 1),
                 Rational(1073741827, 3) + Rational(2, 3))
    assert_equal(Rational(1073741825, 3),
                 Rational(1073741827, 3) - Rational(2, 3))
    assert_equal(Rational(2147483654, 9),
                 Rational(1073741827, 3) * Rational(2, 3))
    assert_equal(Rational(1073741827, 2),
                 Rational(1073741827, 3) / Rational(2, 3))
    assert_equal(Rational(2147483663, 6),
                 Rational(1073741827, 3) + Rational(3, 2))
    assert_equal(Rational(2147483645, 6),
                 Rational(1073741827, 3) - Rational(3, 2))
    assert_equal(Rational(1073741827, 2),
                 Rational(1073741827, 3) * Rational(3, 2))
    assert_equal(Rational(2147483654, 9),
                 Rational(1073741827, 3) / Rational(3, 2))
    assert_equal(Rational(1152921470247108512, 3221225367),
                 Rational(1073741827, 3) + Rational(3, 1073741789))
    assert_equal(Rational(1152921470247108494, 3221225367),
                 Rational(1073741827, 3) - Rational(3, 1073741789))
    assert_equal(Rational(1073741827, 1073741789),
                 Rational(1073741827, 3) * Rational(3, 1073741789))
    assert_equal(Rational(1152921470247108503, 9),
                 Rational(1073741827, 3) / Rational(3, 1073741789))
    assert_equal(Rational(715827872, 1),
                 Rational(1073741827, 3) + Rational(1073741789, 3))
    assert_equal(Rational(38, 3),
                 Rational(1073741827, 3) - Rational(1073741789, 3))
    assert_equal(Rational(1152921470247108503, 9),
                 Rational(1073741827, 3) * Rational(1073741789, 3))
    assert_equal(Rational(1073741827, 1073741789),
                 Rational(1073741827, 3) / Rational(1073741789, 3))
    assert_equal(Rational(1152921511049297938, 3221225481),
                 Rational(1073741827, 3) + Rational(3, 1073741827))
    assert_equal(Rational(1152921511049297920, 3221225481),
                 Rational(1073741827, 3) - Rational(3, 1073741827))
    assert_equal(Rational(1, 1),
                 Rational(1073741827, 3) * Rational(3, 1073741827))
    assert_equal(Rational(1152921511049297929, 9),
                 Rational(1073741827, 3) / Rational(3, 1073741827))
    assert_equal(Rational(2147483654, 3),
                 Rational(1073741827, 3) + Rational(1073741827, 3))
    assert_equal(Rational(0, 1),
                 Rational(1073741827, 3) - Rational(1073741827, 3))
    assert_equal(Rational(1152921511049297929, 9),
                 Rational(1073741827, 3) * Rational(1073741827, 3))
    assert_equal(Rational(1, 1),
                 Rational(1073741827, 3) / Rational(1073741827, 3))
    assert_equal(Rational(1152921514270523296, 3221225481),
                 Rational(1073741827, 3) + Rational(1073741789, 1073741827))
    assert_equal(Rational(1152921507828072562, 3221225481),
                 Rational(1073741827, 3) - Rational(1073741789, 1073741827))
    assert_equal(Rational(1073741789, 3),
                 Rational(1073741827, 3) * Rational(1073741789, 1073741827))
    assert_equal(Rational(1152921511049297929, 3221225367),
                 Rational(1073741827, 3) / Rational(1073741789, 1073741827))
    assert_equal(Rational(1152921473468333984, 3221225367),
                 Rational(1073741827, 3) + Rational(1073741827, 1073741789))
    assert_equal(Rational(1152921467025883022, 3221225367),
                 Rational(1073741827, 3) - Rational(1073741827, 1073741789))
    assert_equal(Rational(1152921511049297929, 3221225367),
                 Rational(1073741827, 3) * Rational(1073741827, 1073741789))
    assert_equal(Rational(1073741789, 3),
                 Rational(1073741827, 3) / Rational(1073741827, 1073741789))
    assert_equal(Rational(1073741789, 1073741827), +Rational(1073741789, 1073741827))
    assert_equal(Rational(-1073741789, 1073741827), -Rational(1073741789, 1073741827))
    assert_equal(Rational(2147483616, 1073741827),
                 Rational(1073741789, 1073741827) + Rational(1, 1))
    assert_equal(Rational(-38, 1073741827),
                 Rational(1073741789, 1073741827) - Rational(1, 1))
    assert_equal(Rational(1073741789, 1073741827),
                 Rational(1073741789, 1073741827) * Rational(1, 1))
    assert_equal(Rational(1073741789, 1073741827),
                 Rational(1073741789, 1073741827) / Rational(1, 1))
    assert_equal(Rational(3221225443, 1073741827),
                 Rational(1073741789, 1073741827) + Rational(2, 1))
    assert_equal(Rational(-1073741865, 1073741827),
                 Rational(1073741789, 1073741827) - Rational(2, 1))
    assert_equal(Rational(2147483578, 1073741827),
                 Rational(1073741789, 1073741827) * Rational(2, 1))
    assert_equal(Rational(1073741789, 2147483654),
                 Rational(1073741789, 1073741827) / Rational(2, 1))
    assert_equal(Rational(4294967270, 1073741827),
                 Rational(1073741789, 1073741827) + Rational(3, 1))
    assert_equal(Rational(-2147483692, 1073741827),
                 Rational(1073741789, 1073741827) - Rational(3, 1))
    assert_equal(Rational(3221225367, 1073741827),
                 Rational(1073741789, 1073741827) * Rational(3, 1))
    assert_equal(Rational(1073741789, 3221225481),
                 Rational(1073741789, 1073741827) / Rational(3, 1))
    assert_equal(Rational(1152921471320850292, 1073741827),
                 Rational(1073741789, 1073741827) + Rational(1073741789, 1))
    assert_equal(Rational(-1152921469173366714, 1073741827),
                 Rational(1073741789, 1073741827) - Rational(1073741789, 1))
    assert_equal(Rational(1152921429444920521, 1073741827),
                 Rational(1073741789, 1073741827) * Rational(1073741789, 1))
    assert_equal(Rational(1, 1073741827),
                 Rational(1073741789, 1073741827) / Rational(1073741789, 1))
    assert_equal(Rational(1152921512123039718, 1073741827),
                 Rational(1073741789, 1073741827) + Rational(1073741827, 1))
    assert_equal(Rational(-1152921509975556140, 1073741827),
                 Rational(1073741789, 1073741827) - Rational(1073741827, 1))
    assert_equal(Rational(1073741789, 1),
                 Rational(1073741789, 1073741827) * Rational(1073741827, 1))
    assert_equal(Rational(1073741789, 1152921511049297929),
                 Rational(1073741789, 1073741827) / Rational(1073741827, 1))
    assert_equal(Rational(5368709021, 3221225481),
                 Rational(1073741789, 1073741827) + Rational(2, 3))
    assert_equal(Rational(1073741713, 3221225481),
                 Rational(1073741789, 1073741827) - Rational(2, 3))
    assert_equal(Rational(2147483578, 3221225481),
                 Rational(1073741789, 1073741827) * Rational(2, 3))
    assert_equal(Rational(3221225367, 2147483654),
                 Rational(1073741789, 1073741827) / Rational(2, 3))
    assert_equal(Rational(5368709059, 2147483654),
                 Rational(1073741789, 1073741827) + Rational(3, 2))
    assert_equal(Rational(-1073741903, 2147483654),
                 Rational(1073741789, 1073741827) - Rational(3, 2))
    assert_equal(Rational(3221225367, 2147483654),
                 Rational(1073741789, 1073741827) * Rational(3, 2))
    assert_equal(Rational(2147483578, 3221225481),
                 Rational(1073741789, 1073741827) / Rational(3, 2))
    assert_equal(Rational(1152921432666146002, 1152921470247108503),
                 Rational(1073741789, 1073741827) + Rational(3, 1073741789))
    assert_equal(Rational(1152921426223695040, 1152921470247108503),
                 Rational(1073741789, 1073741827) - Rational(3, 1073741789))
    assert_equal(Rational(3, 1073741827),
                 Rational(1073741789, 1073741827) * Rational(3, 1073741789))
    assert_equal(Rational(1152921429444920521, 3221225481),
                 Rational(1073741789, 1073741827) / Rational(3, 1073741789))
    assert_equal(Rational(1152921473468333870, 3221225481),
                 Rational(1073741789, 1073741827) + Rational(1073741789, 3))
    assert_equal(Rational(-1152921467025883136, 3221225481),
                 Rational(1073741789, 1073741827) - Rational(1073741789, 3))
    assert_equal(Rational(1152921429444920521, 3221225481),
                 Rational(1073741789, 1073741827) * Rational(1073741789, 3))
    assert_equal(Rational(3, 1073741827),
                 Rational(1073741789, 1073741827) / Rational(1073741789, 3))
    assert_equal(Rational(1073741792, 1073741827),
                 Rational(1073741789, 1073741827) + Rational(3, 1073741827))
    assert_equal(Rational(1073741786, 1073741827),
                 Rational(1073741789, 1073741827) - Rational(3, 1073741827))
    assert_equal(Rational(3221225367, 1152921511049297929),
                 Rational(1073741789, 1073741827) * Rational(3, 1073741827))
    assert_equal(Rational(1073741789, 3),
                 Rational(1073741789, 1073741827) / Rational(3, 1073741827))
    assert_equal(Rational(1152921514270523296, 3221225481),
                 Rational(1073741789, 1073741827) + Rational(1073741827, 3))
    assert_equal(Rational(-1152921507828072562, 3221225481),
                 Rational(1073741789, 1073741827) - Rational(1073741827, 3))
    assert_equal(Rational(1073741789, 3),
                 Rational(1073741789, 1073741827) * Rational(1073741827, 3))
    assert_equal(Rational(3221225367, 1152921511049297929),
                 Rational(1073741789, 1073741827) / Rational(1073741827, 3))
    assert_equal(Rational(2147483578, 1073741827),
                 Rational(1073741789, 1073741827) + Rational(1073741789, 1073741827))
    assert_equal(Rational(0, 1),
                 Rational(1073741789, 1073741827) - Rational(1073741789, 1073741827))
    assert_equal(Rational(1152921429444920521, 1152921511049297929),
                 Rational(1073741789, 1073741827) * Rational(1073741789, 1073741827))
    assert_equal(Rational(1, 1),
                 Rational(1073741789, 1073741827) / Rational(1073741789, 1073741827))
    assert_equal(Rational(2305842940494218450, 1152921470247108503),
                 Rational(1073741789, 1073741827) + Rational(1073741827, 1073741789))
    assert_equal(Rational(-81604377408, 1152921470247108503),
                 Rational(1073741789, 1073741827) - Rational(1073741827, 1073741789))
    assert_equal(Rational(1, 1),
                 Rational(1073741789, 1073741827) * Rational(1073741827, 1073741789))
    assert_equal(Rational(1152921429444920521, 1152921511049297929),
                 Rational(1073741789, 1073741827) / Rational(1073741827, 1073741789))
    assert_equal(Rational(1073741827, 1073741789), +Rational(1073741827, 1073741789))
    assert_equal(Rational(-1073741827, 1073741789), -Rational(1073741827, 1073741789))
    assert_equal(Rational(2147483616, 1073741789),
                 Rational(1073741827, 1073741789) + Rational(1, 1))
    assert_equal(Rational(38, 1073741789),
                 Rational(1073741827, 1073741789) - Rational(1, 1))
    assert_equal(Rational(1073741827, 1073741789),
                 Rational(1073741827, 1073741789) * Rational(1, 1))
    assert_equal(Rational(1073741827, 1073741789),
                 Rational(1073741827, 1073741789) / Rational(1, 1))
    assert_equal(Rational(3221225405, 1073741789),
                 Rational(1073741827, 1073741789) + Rational(2, 1))
    assert_equal(Rational(-1073741751, 1073741789),
                 Rational(1073741827, 1073741789) - Rational(2, 1))
    assert_equal(Rational(2147483654, 1073741789),
                 Rational(1073741827, 1073741789) * Rational(2, 1))
    assert_equal(Rational(1073741827, 2147483578),
                 Rational(1073741827, 1073741789) / Rational(2, 1))
    assert_equal(Rational(4294967194, 1073741789),
                 Rational(1073741827, 1073741789) + Rational(3, 1))
    assert_equal(Rational(-2147483540, 1073741789),
                 Rational(1073741827, 1073741789) - Rational(3, 1))
    assert_equal(Rational(3221225481, 1073741789),
                 Rational(1073741827, 1073741789) * Rational(3, 1))
    assert_equal(Rational(1073741827, 3221225367),
                 Rational(1073741827, 1073741789) / Rational(3, 1))
    assert_equal(Rational(1152921430518662348, 1073741789),
                 Rational(1073741827, 1073741789) + Rational(1073741789, 1))
    assert_equal(Rational(-1152921428371178694, 1073741789),
                 Rational(1073741827, 1073741789) - Rational(1073741789, 1))
    assert_equal(Rational(1073741827, 1),
                 Rational(1073741827, 1073741789) * Rational(1073741789, 1))
    assert_equal(Rational(1073741827, 1152921429444920521),
                 Rational(1073741827, 1073741789) / Rational(1073741789, 1))
    assert_equal(Rational(1152921471320850330, 1073741789),
                 Rational(1073741827, 1073741789) + Rational(1073741827, 1))
    assert_equal(Rational(-1152921469173366676, 1073741789),
                 Rational(1073741827, 1073741789) - Rational(1073741827, 1))
    assert_equal(Rational(1152921511049297929, 1073741789),
                 Rational(1073741827, 1073741789) * Rational(1073741827, 1))
    assert_equal(Rational(1, 1073741789),
                 Rational(1073741827, 1073741789) / Rational(1073741827, 1))
    assert_equal(Rational(5368709059, 3221225367),
                 Rational(1073741827, 1073741789) + Rational(2, 3))
    assert_equal(Rational(1073741903, 3221225367),
                 Rational(1073741827, 1073741789) - Rational(2, 3))
    assert_equal(Rational(2147483654, 3221225367),
                 Rational(1073741827, 1073741789) * Rational(2, 3))
    assert_equal(Rational(3221225481, 2147483578),
                 Rational(1073741827, 1073741789) / Rational(2, 3))
    assert_equal(Rational(5368709021, 2147483578),
                 Rational(1073741827, 1073741789) + Rational(3, 2))
    assert_equal(Rational(-1073741713, 2147483578),
                 Rational(1073741827, 1073741789) - Rational(3, 2))
    assert_equal(Rational(3221225481, 2147483578),
                 Rational(1073741827, 1073741789) * Rational(3, 2))
    assert_equal(Rational(2147483654, 3221225367),
                 Rational(1073741827, 1073741789) / Rational(3, 2))
    assert_equal(Rational(1073741830, 1073741789),
                 Rational(1073741827, 1073741789) + Rational(3, 1073741789))
    assert_equal(Rational(1073741824, 1073741789),
                 Rational(1073741827, 1073741789) - Rational(3, 1073741789))
    assert_equal(Rational(3221225481, 1152921429444920521),
                 Rational(1073741827, 1073741789) * Rational(3, 1073741789))
    assert_equal(Rational(1073741827, 3),
                 Rational(1073741827, 1073741789) / Rational(3, 1073741789))
    assert_equal(Rational(1152921432666146002, 3221225367),
                 Rational(1073741827, 1073741789) + Rational(1073741789, 3))
    assert_equal(Rational(-1152921426223695040, 3221225367),
                 Rational(1073741827, 1073741789) - Rational(1073741789, 3))
    assert_equal(Rational(1073741827, 3),
                 Rational(1073741827, 1073741789) * Rational(1073741789, 3))
    assert_equal(Rational(3221225481, 1152921429444920521),
                 Rational(1073741827, 1073741789) / Rational(1073741789, 3))
    assert_equal(Rational(1152921514270523296, 1152921470247108503),
                 Rational(1073741827, 1073741789) + Rational(3, 1073741827))
    assert_equal(Rational(1152921507828072562, 1152921470247108503),
                 Rational(1073741827, 1073741789) - Rational(3, 1073741827))
    assert_equal(Rational(3, 1073741789),
                 Rational(1073741827, 1073741789) * Rational(3, 1073741827))
    assert_equal(Rational(1152921511049297929, 3221225367),
                 Rational(1073741827, 1073741789) / Rational(3, 1073741827))
    assert_equal(Rational(1152921473468333984, 3221225367),
                 Rational(1073741827, 1073741789) + Rational(1073741827, 3))
    assert_equal(Rational(-1152921467025883022, 3221225367),
                 Rational(1073741827, 1073741789) - Rational(1073741827, 3))
    assert_equal(Rational(1152921511049297929, 3221225367),
                 Rational(1073741827, 1073741789) * Rational(1073741827, 3))
    assert_equal(Rational(3, 1073741789),
                 Rational(1073741827, 1073741789) / Rational(1073741827, 3))
    assert_equal(Rational(2305842940494218450, 1152921470247108503),
                 Rational(1073741827, 1073741789) + Rational(1073741789, 1073741827))
    assert_equal(Rational(81604377408, 1152921470247108503),
                 Rational(1073741827, 1073741789) - Rational(1073741789, 1073741827))
    assert_equal(Rational(1, 1),
                 Rational(1073741827, 1073741789) * Rational(1073741789, 1073741827))
    assert_equal(Rational(1152921511049297929, 1152921429444920521),
                 Rational(1073741827, 1073741789) / Rational(1073741789, 1073741827))
    assert_equal(Rational(2147483654, 1073741789),
                 Rational(1073741827, 1073741789) + Rational(1073741827, 1073741789))
    assert_equal(Rational(0, 1),
                 Rational(1073741827, 1073741789) - Rational(1073741827, 1073741789))
    assert_equal(Rational(1152921511049297929, 1152921429444920521),
                 Rational(1073741827, 1073741789) * Rational(1073741827, 1073741789))
    assert_equal(Rational(1, 1),
                 Rational(1073741827, 1073741789) / Rational(1073741827, 1073741789))
  end

end
