require 'test/unit'

class Complex_Test2 < Test::Unit::TestCase

  def test_kumi
    skip unless defined?(Rational)

    assert_equal(Complex(1, 0), +Complex(1, 0))
    assert_equal(Complex(-1, 0), -Complex(1, 0))
    assert_equal(Complex(2, 0),
                 Complex(1, 0) + Complex(1, 0))
    assert_equal(Complex(0, 0),
                 Complex(1, 0) - Complex(1, 0))
    assert_equal(Complex(1, 0),
                 Complex(1, 0) * Complex(1, 0))
    assert_equal(Complex(1, 0),
                 Complex(1, 0) / Complex(1, 0))
    assert_equal(Complex(1073741790, 0),
                 Complex(1, 0) + Complex(1073741789, 0))
    assert_equal(Complex(-1073741788, 0),
                 Complex(1, 0) - Complex(1073741789, 0))
    assert_equal(Complex(1073741789, 0),
                 Complex(1, 0) * Complex(1073741789, 0))
    assert_equal(Complex(Rational(1, 1073741789), 0),
                 Complex(1, 0) / Complex(1073741789, 0))
    assert_equal(Complex(1073741828, 0),
                 Complex(1, 0) + Complex(1073741827, 0))
    assert_equal(Complex(-1073741826, 0),
                 Complex(1, 0) - Complex(1073741827, 0))
    assert_equal(Complex(1073741827, 0),
                 Complex(1, 0) * Complex(1073741827, 0))
    assert_equal(Complex(Rational(1, 1073741827), 0),
                 Complex(1, 0) / Complex(1073741827, 0))
    assert_equal(Complex(1073741790, 1073741789),
                 Complex(1, 0) + Complex(1073741789, 1073741789))
    assert_equal(Complex(-1073741788, -1073741789),
                 Complex(1, 0) - Complex(1073741789, 1073741789))
    assert_equal(Complex(1073741789, 1073741789),
                 Complex(1, 0) * Complex(1073741789, 1073741789))
    assert_equal(Complex(Rational(1, 2147483578), Rational(-1, 2147483578)),
                 Complex(1, 0) / Complex(1073741789, 1073741789))
    assert_equal(Complex(1073741790, 1073741827),
                 Complex(1, 0) + Complex(1073741789, 1073741827))
    assert_equal(Complex(-1073741788, -1073741827),
                 Complex(1, 0) - Complex(1073741789, 1073741827))
    assert_equal(Complex(1073741789, 1073741827),
                 Complex(1, 0) * Complex(1073741789, 1073741827))
    assert_equal(Complex(Rational(1073741789, 2305842940494218450), Rational(-1073741827, 2305842940494218450)),
                 Complex(1, 0) / Complex(1073741789, 1073741827))
    assert_equal(Complex(1073741828, 1073741827),
                 Complex(1, 0) + Complex(1073741827, 1073741827))
    assert_equal(Complex(-1073741826, -1073741827),
                 Complex(1, 0) - Complex(1073741827, 1073741827))
    assert_equal(Complex(1073741827, 1073741827),
                 Complex(1, 0) * Complex(1073741827, 1073741827))
    assert_equal(Complex(Rational(1, 2147483654), Rational(-1, 2147483654)),
                 Complex(1, 0) / Complex(1073741827, 1073741827))
    assert_equal(Complex(Rational(2147483616, 1073741827), Rational(1073741789, 1073741827)),
                 Complex(1, 0) + Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(38, 1073741827), Rational(-1073741789, 1073741827)),
                 Complex(1, 0) - Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)),
                 Complex(1, 0) * Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(1073741827, 2147483578), Rational(-1073741827, 2147483578)),
                 Complex(1, 0) / Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(2147483616, 1073741789), Rational(1073741827, 1073741789)),
                 Complex(1, 0) + Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(-38, 1073741789), Rational(-1073741827, 1073741789)),
                 Complex(1, 0) - Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)),
                 Complex(1, 0) * Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1073741789, 2147483654), Rational(-1073741789, 2147483654)),
                 Complex(1, 0) / Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(2147483616, 1073741827), Rational(1073741827, 1073741789)),
                 Complex(1, 0) + Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(38, 1073741827), Rational(-1073741827, 1073741789)),
                 Complex(1, 0) - Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)),
                 Complex(1, 0) * Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1329227869515035739611240300898290063, 2658455833113515253509575011810600482), Rational(-1329227963598474519442525600436190287, 2658455833113515253509575011810600482)),
                 Complex(1, 0) / Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(1073741789, 0), +Complex(1073741789, 0))
    assert_equal(Complex(-1073741789, 0), -Complex(1073741789, 0))
    assert_equal(Complex(1073741790, 0),
                 Complex(1073741789, 0) + Complex(1, 0))
    assert_equal(Complex(1073741788, 0),
                 Complex(1073741789, 0) - Complex(1, 0))
    assert_equal(Complex(1073741789, 0),
                 Complex(1073741789, 0) * Complex(1, 0))
    assert_equal(Complex(1073741789, 0),
                 Complex(1073741789, 0) / Complex(1, 0))
    assert_equal(Complex(2147483578, 0),
                 Complex(1073741789, 0) + Complex(1073741789, 0))
    assert_equal(Complex(0, 0),
                 Complex(1073741789, 0) - Complex(1073741789, 0))
    assert_equal(Complex(1152921429444920521, 0),
                 Complex(1073741789, 0) * Complex(1073741789, 0))
    assert_equal(Complex(1, 0),
                 Complex(1073741789, 0) / Complex(1073741789, 0))
    assert_equal(Complex(2147483616, 0),
                 Complex(1073741789, 0) + Complex(1073741827, 0))
    assert_equal(Complex(-38, 0),
                 Complex(1073741789, 0) - Complex(1073741827, 0))
    assert_equal(Complex(1152921470247108503, 0),
                 Complex(1073741789, 0) * Complex(1073741827, 0))
    assert_equal(Complex(Rational(1073741789, 1073741827), 0),
                 Complex(1073741789, 0) / Complex(1073741827, 0))
    assert_equal(Complex(2147483578, 1073741789),
                 Complex(1073741789, 0) + Complex(1073741789, 1073741789))
    assert_equal(Complex(0, -1073741789),
                 Complex(1073741789, 0) - Complex(1073741789, 1073741789))
    assert_equal(Complex(1152921429444920521, 1152921429444920521),
                 Complex(1073741789, 0) * Complex(1073741789, 1073741789))
    assert_equal(Complex(Rational(1, 2), Rational(-1, 2)),
                 Complex(1073741789, 0) / Complex(1073741789, 1073741789))
    assert_equal(Complex(2147483578, 1073741827),
                 Complex(1073741789, 0) + Complex(1073741789, 1073741827))
    assert_equal(Complex(0, -1073741827),
                 Complex(1073741789, 0) - Complex(1073741789, 1073741827))
    assert_equal(Complex(1152921429444920521, 1152921470247108503),
                 Complex(1073741789, 0) * Complex(1073741789, 1073741827))
    assert_equal(Complex(Rational(1152921429444920521, 2305842940494218450), Rational(-1152921470247108503, 2305842940494218450)),
                 Complex(1073741789, 0) / Complex(1073741789, 1073741827))
    assert_equal(Complex(2147483616, 1073741827),
                 Complex(1073741789, 0) + Complex(1073741827, 1073741827))
    assert_equal(Complex(-38, -1073741827),
                 Complex(1073741789, 0) - Complex(1073741827, 1073741827))
    assert_equal(Complex(1152921470247108503, 1152921470247108503),
                 Complex(1073741789, 0) * Complex(1073741827, 1073741827))
    assert_equal(Complex(Rational(1073741789, 2147483654), Rational(-1073741789, 2147483654)),
                 Complex(1073741789, 0) / Complex(1073741827, 1073741827))
    assert_equal(Complex(Rational(1152921471320850292, 1073741827), Rational(1073741789, 1073741827)),
                 Complex(1073741789, 0) + Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(1152921469173366714, 1073741827), Rational(-1073741789, 1073741827)),
                 Complex(1073741789, 0) - Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(1152921429444920521, 1073741827), Rational(1152921429444920521, 1073741827)),
                 Complex(1073741789, 0) * Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(1073741827, 2), Rational(-1073741827, 2)),
                 Complex(1073741789, 0) / Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(1152921430518662348, 1073741789), Rational(1073741827, 1073741789)),
                 Complex(1073741789, 0) + Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1152921428371178694, 1073741789), Rational(-1073741827, 1073741789)),
                 Complex(1073741789, 0) - Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(1073741827, 1073741827),
                 Complex(1073741789, 0) * Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1152921429444920521, 2147483654), Rational(-1152921429444920521, 2147483654)),
                 Complex(1073741789, 0) / Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1152921471320850292, 1073741827), Rational(1073741827, 1073741789)),
                 Complex(1073741789, 0) + Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1152921469173366714, 1073741827), Rational(-1073741827, 1073741789)),
                 Complex(1073741789, 0) - Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1152921429444920521, 1073741827), 1073741827),
                 Complex(1073741789, 0) * Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1427247510601733037449111325195428279286542707, 2658455833113515253509575011810600482), Rational(-1427247611623052908177132720890654139107803443, 2658455833113515253509575011810600482)),
                 Complex(1073741789, 0) / Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(1073741827, 0), +Complex(1073741827, 0))
    assert_equal(Complex(-1073741827, 0), -Complex(1073741827, 0))
    assert_equal(Complex(1073741828, 0),
                 Complex(1073741827, 0) + Complex(1, 0))
    assert_equal(Complex(1073741826, 0),
                 Complex(1073741827, 0) - Complex(1, 0))
    assert_equal(Complex(1073741827, 0),
                 Complex(1073741827, 0) * Complex(1, 0))
    assert_equal(Complex(1073741827, 0),
                 Complex(1073741827, 0) / Complex(1, 0))
    assert_equal(Complex(2147483616, 0),
                 Complex(1073741827, 0) + Complex(1073741789, 0))
    assert_equal(Complex(38, 0),
                 Complex(1073741827, 0) - Complex(1073741789, 0))
    assert_equal(Complex(1152921470247108503, 0),
                 Complex(1073741827, 0) * Complex(1073741789, 0))
    assert_equal(Complex(Rational(1073741827, 1073741789), 0),
                 Complex(1073741827, 0) / Complex(1073741789, 0))
    assert_equal(Complex(2147483654, 0),
                 Complex(1073741827, 0) + Complex(1073741827, 0))
    assert_equal(Complex(0, 0),
                 Complex(1073741827, 0) - Complex(1073741827, 0))
    assert_equal(Complex(1152921511049297929, 0),
                 Complex(1073741827, 0) * Complex(1073741827, 0))
    assert_equal(Complex(1, 0),
                 Complex(1073741827, 0) / Complex(1073741827, 0))
    assert_equal(Complex(2147483616, 1073741789),
                 Complex(1073741827, 0) + Complex(1073741789, 1073741789))
    assert_equal(Complex(38, -1073741789),
                 Complex(1073741827, 0) - Complex(1073741789, 1073741789))
    assert_equal(Complex(1152921470247108503, 1152921470247108503),
                 Complex(1073741827, 0) * Complex(1073741789, 1073741789))
    assert_equal(Complex(Rational(1073741827, 2147483578), Rational(-1073741827, 2147483578)),
                 Complex(1073741827, 0) / Complex(1073741789, 1073741789))
    assert_equal(Complex(2147483616, 1073741827),
                 Complex(1073741827, 0) + Complex(1073741789, 1073741827))
    assert_equal(Complex(38, -1073741827),
                 Complex(1073741827, 0) - Complex(1073741789, 1073741827))
    assert_equal(Complex(1152921470247108503, 1152921511049297929),
                 Complex(1073741827, 0) * Complex(1073741789, 1073741827))
    assert_equal(Complex(Rational(1152921470247108503, 2305842940494218450), Rational(-1152921511049297929, 2305842940494218450)),
                 Complex(1073741827, 0) / Complex(1073741789, 1073741827))
    assert_equal(Complex(2147483654, 1073741827),
                 Complex(1073741827, 0) + Complex(1073741827, 1073741827))
    assert_equal(Complex(0, -1073741827),
                 Complex(1073741827, 0) - Complex(1073741827, 1073741827))
    assert_equal(Complex(1152921511049297929, 1152921511049297929),
                 Complex(1073741827, 0) * Complex(1073741827, 1073741827))
    assert_equal(Complex(Rational(1, 2), Rational(-1, 2)),
                 Complex(1073741827, 0) / Complex(1073741827, 1073741827))
    assert_equal(Complex(Rational(1152921512123039718, 1073741827), Rational(1073741789, 1073741827)),
                 Complex(1073741827, 0) + Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(1152921509975556140, 1073741827), Rational(-1073741789, 1073741827)),
                 Complex(1073741827, 0) - Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(1073741789, 1073741789),
                 Complex(1073741827, 0) * Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(1152921511049297929, 2147483578), Rational(-1152921511049297929, 2147483578)),
                 Complex(1073741827, 0) / Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(1152921471320850330, 1073741789), Rational(1073741827, 1073741789)),
                 Complex(1073741827, 0) + Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1152921469173366676, 1073741789), Rational(-1073741827, 1073741789)),
                 Complex(1073741827, 0) - Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1152921511049297929, 1073741789), Rational(1152921511049297929, 1073741789)),
                 Complex(1073741827, 0) * Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1073741789, 2), Rational(-1073741789, 2)),
                 Complex(1073741827, 0) / Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1152921512123039718, 1073741827), Rational(1073741827, 1073741789)),
                 Complex(1073741827, 0) + Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1152921509975556140, 1073741827), Rational(-1073741827, 1073741789)),
                 Complex(1073741827, 0) - Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(1073741789, Rational(1152921511049297929, 1073741789)),
                 Complex(1073741827, 0) * Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1427247561112392079020469430422559713421565101, 2658455833113515253509575011810600482), Rational(-1427247662133715524919164459706626955683034349, 2658455833113515253509575011810600482)),
                 Complex(1073741827, 0) / Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(1073741789, 1073741789), +Complex(1073741789, 1073741789))
    assert_equal(Complex(-1073741789, -1073741789), -Complex(1073741789, 1073741789))
    assert_equal(Complex(1073741790, 1073741789),
                 Complex(1073741789, 1073741789) + Complex(1, 0))
    assert_equal(Complex(1073741788, 1073741789),
                 Complex(1073741789, 1073741789) - Complex(1, 0))
    assert_equal(Complex(1073741789, 1073741789),
                 Complex(1073741789, 1073741789) * Complex(1, 0))
    assert_equal(Complex(1073741789, 1073741789),
                 Complex(1073741789, 1073741789) / Complex(1, 0))
    assert_equal(Complex(2147483578, 1073741789),
                 Complex(1073741789, 1073741789) + Complex(1073741789, 0))
    assert_equal(Complex(0, 1073741789),
                 Complex(1073741789, 1073741789) - Complex(1073741789, 0))
    assert_equal(Complex(1152921429444920521, 1152921429444920521),
                 Complex(1073741789, 1073741789) * Complex(1073741789, 0))
    assert_equal(Complex(1, 1),
                 Complex(1073741789, 1073741789) / Complex(1073741789, 0))
    assert_equal(Complex(2147483616, 1073741789),
                 Complex(1073741789, 1073741789) + Complex(1073741827, 0))
    assert_equal(Complex(-38, 1073741789),
                 Complex(1073741789, 1073741789) - Complex(1073741827, 0))
    assert_equal(Complex(1152921470247108503, 1152921470247108503),
                 Complex(1073741789, 1073741789) * Complex(1073741827, 0))
    assert_equal(Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)),
                 Complex(1073741789, 1073741789) / Complex(1073741827, 0))
    assert_equal(Complex(2147483578, 2147483578),
                 Complex(1073741789, 1073741789) + Complex(1073741789, 1073741789))
    assert_equal(Complex(0, 0),
                 Complex(1073741789, 1073741789) - Complex(1073741789, 1073741789))
    assert_equal(Complex(0, 2305842858889841042),
                 Complex(1073741789, 1073741789) * Complex(1073741789, 1073741789))
    assert_equal(Complex(1, 0),
                 Complex(1073741789, 1073741789) / Complex(1073741789, 1073741789))
    assert_equal(Complex(2147483578, 2147483616),
                 Complex(1073741789, 1073741789) + Complex(1073741789, 1073741827))
    assert_equal(Complex(0, -38),
                 Complex(1073741789, 1073741789) - Complex(1073741789, 1073741827))
    assert_equal(Complex(-40802187982, 2305842899692029024),
                 Complex(1073741789, 1073741789) * Complex(1073741789, 1073741827))
    assert_equal(Complex(Rational(1152921449846014512, 1152921470247109225), Rational(-20401093991, 1152921470247109225)),
                 Complex(1073741789, 1073741789) / Complex(1073741789, 1073741827))
    assert_equal(Complex(2147483616, 2147483616),
                 Complex(1073741789, 1073741789) + Complex(1073741827, 1073741827))
    assert_equal(Complex(-38, -38),
                 Complex(1073741789, 1073741789) - Complex(1073741827, 1073741827))
    assert_equal(Complex(0, 2305842940494217006),
                 Complex(1073741789, 1073741789) * Complex(1073741827, 1073741827))
    assert_equal(Complex(Rational(1073741789, 1073741827), 0),
                 Complex(1073741789, 1073741789) / Complex(1073741827, 1073741827))
    assert_equal(Complex(Rational(1152921471320850292, 1073741827), Rational(1152921471320850292, 1073741827)),
                 Complex(1073741789, 1073741789) + Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(1152921469173366714, 1073741827), Rational(1152921469173366714, 1073741827)),
                 Complex(1073741789, 1073741789) - Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(0, Rational(2305842858889841042, 1073741827)),
                 Complex(1073741789, 1073741789) * Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(1073741827, 0),
                 Complex(1073741789, 1073741789) / Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(1152921430518662348, 1073741789), Rational(1152921430518662348, 1073741789)),
                 Complex(1073741789, 1073741789) + Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1152921428371178694, 1073741789), Rational(1152921428371178694, 1073741789)),
                 Complex(1073741789, 1073741789) - Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(0, 2147483654),
                 Complex(1073741789, 1073741789) * Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1152921429444920521, 1073741827), 0),
                 Complex(1073741789, 1073741789) / Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1152921471320850292, 1073741827), Rational(1152921430518662348, 1073741789)),
                 Complex(1073741789, 1073741789) + Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1152921469173366714, 1073741827), Rational(1152921428371178694, 1073741789)),
                 Complex(1073741789, 1073741789) - Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(-81604377408, 1073741827), Rational(2305842940494218450, 1073741827)),
                 Complex(1073741789, 1073741789) * Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1427247561112392972813122023043041209197173075, 1329227916556757626754787505905300241), Rational(-50510659935364010697847612929910630368, 1329227916556757626754787505905300241)),
                 Complex(1073741789, 1073741789) / Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(1073741789, 1073741827), +Complex(1073741789, 1073741827))
    assert_equal(Complex(-1073741789, -1073741827), -Complex(1073741789, 1073741827))
    assert_equal(Complex(1073741790, 1073741827),
                 Complex(1073741789, 1073741827) + Complex(1, 0))
    assert_equal(Complex(1073741788, 1073741827),
                 Complex(1073741789, 1073741827) - Complex(1, 0))
    assert_equal(Complex(1073741789, 1073741827),
                 Complex(1073741789, 1073741827) * Complex(1, 0))
    assert_equal(Complex(1073741789, 1073741827),
                 Complex(1073741789, 1073741827) / Complex(1, 0))
    assert_equal(Complex(2147483578, 1073741827),
                 Complex(1073741789, 1073741827) + Complex(1073741789, 0))
    assert_equal(Complex(0, 1073741827),
                 Complex(1073741789, 1073741827) - Complex(1073741789, 0))
    assert_equal(Complex(1152921429444920521, 1152921470247108503),
                 Complex(1073741789, 1073741827) * Complex(1073741789, 0))
    assert_equal(Complex(1, Rational(1073741827, 1073741789)),
                 Complex(1073741789, 1073741827) / Complex(1073741789, 0))
    assert_equal(Complex(2147483616, 1073741827),
                 Complex(1073741789, 1073741827) + Complex(1073741827, 0))
    assert_equal(Complex(-38, 1073741827),
                 Complex(1073741789, 1073741827) - Complex(1073741827, 0))
    assert_equal(Complex(1152921470247108503, 1152921511049297929),
                 Complex(1073741789, 1073741827) * Complex(1073741827, 0))
    assert_equal(Complex(Rational(1073741789, 1073741827), 1),
                 Complex(1073741789, 1073741827) / Complex(1073741827, 0))
    assert_equal(Complex(2147483578, 2147483616),
                 Complex(1073741789, 1073741827) + Complex(1073741789, 1073741789))
    assert_equal(Complex(0, 38),
                 Complex(1073741789, 1073741827) - Complex(1073741789, 1073741789))
    assert_equal(Complex(-40802187982, 2305842899692029024),
                 Complex(1073741789, 1073741827) * Complex(1073741789, 1073741789))
    assert_equal(Complex(Rational(1073741808, 1073741789), Rational(19, 1073741789)),
                 Complex(1073741789, 1073741827) / Complex(1073741789, 1073741789))
    assert_equal(Complex(2147483578, 2147483654),
                 Complex(1073741789, 1073741827) + Complex(1073741789, 1073741827))
    assert_equal(Complex(0, 0),
                 Complex(1073741789, 1073741827) - Complex(1073741789, 1073741827))
    assert_equal(Complex(-81604377408, 2305842940494217006),
                 Complex(1073741789, 1073741827) * Complex(1073741789, 1073741827))
    assert_equal(Complex(1, 0),
                 Complex(1073741789, 1073741827) / Complex(1073741789, 1073741827))
    assert_equal(Complex(2147483616, 2147483654),
                 Complex(1073741789, 1073741827) + Complex(1073741827, 1073741827))
    assert_equal(Complex(-38, 0),
                 Complex(1073741789, 1073741827) - Complex(1073741827, 1073741827))
    assert_equal(Complex(-40802189426, 2305842981296406432),
                 Complex(1073741789, 1073741827) * Complex(1073741827, 1073741827))
    assert_equal(Complex(Rational(1073741808, 1073741827), Rational(19, 1073741827)),
                 Complex(1073741789, 1073741827) / Complex(1073741827, 1073741827))
    assert_equal(Complex(Rational(1152921471320850292, 1073741827), Rational(1152921512123039718, 1073741827)),
                 Complex(1073741789, 1073741827) + Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(1152921469173366714, 1073741827), Rational(1152921509975556140, 1073741827)),
                 Complex(1073741789, 1073741827) - Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(-40802187982, 1073741827), Rational(2305842899692029024, 1073741827)),
                 Complex(1073741789, 1073741827) * Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(1152921490648203216, 1073741789), Rational(20401094713, 1073741789)),
                 Complex(1073741789, 1073741827) / Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(1152921430518662348, 1073741789), Rational(1152921471320850330, 1073741789)),
                 Complex(1073741789, 1073741827) + Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1152921428371178694, 1073741789), Rational(1152921469173366676, 1073741789)),
                 Complex(1073741789, 1073741827) - Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(-40802189426, 1073741789), Rational(2305842981296406432, 1073741789)),
                 Complex(1073741789, 1073741827) * Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1152921449846014512, 1073741827), Rational(20401093991, 1073741827)),
                 Complex(1073741789, 1073741827) / Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1152921471320850292, 1073741827), Rational(1152921471320850330, 1073741789)),
                 Complex(1073741789, 1073741827) + Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1152921469173366714, 1073741827), Rational(1152921469173366676, 1073741789)),
                 Complex(1073741789, 1073741827) - Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(-131433047608170424214, 1152921470247108503), 2147483616),
                 Complex(1073741789, 1073741827) * Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1427247586367724281184137892451027617484788528, 1329227916556757626754787505905300241), Rational(-25255330414578331645234047212843119171, 1329227916556757626754787505905300241)),
                 Complex(1073741789, 1073741827) / Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(1073741827, 1073741827), +Complex(1073741827, 1073741827))
    assert_equal(Complex(-1073741827, -1073741827), -Complex(1073741827, 1073741827))
    assert_equal(Complex(1073741828, 1073741827),
                 Complex(1073741827, 1073741827) + Complex(1, 0))
    assert_equal(Complex(1073741826, 1073741827),
                 Complex(1073741827, 1073741827) - Complex(1, 0))
    assert_equal(Complex(1073741827, 1073741827),
                 Complex(1073741827, 1073741827) * Complex(1, 0))
    assert_equal(Complex(1073741827, 1073741827),
                 Complex(1073741827, 1073741827) / Complex(1, 0))
    assert_equal(Complex(2147483616, 1073741827),
                 Complex(1073741827, 1073741827) + Complex(1073741789, 0))
    assert_equal(Complex(38, 1073741827),
                 Complex(1073741827, 1073741827) - Complex(1073741789, 0))
    assert_equal(Complex(1152921470247108503, 1152921470247108503),
                 Complex(1073741827, 1073741827) * Complex(1073741789, 0))
    assert_equal(Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)),
                 Complex(1073741827, 1073741827) / Complex(1073741789, 0))
    assert_equal(Complex(2147483654, 1073741827),
                 Complex(1073741827, 1073741827) + Complex(1073741827, 0))
    assert_equal(Complex(0, 1073741827),
                 Complex(1073741827, 1073741827) - Complex(1073741827, 0))
    assert_equal(Complex(1152921511049297929, 1152921511049297929),
                 Complex(1073741827, 1073741827) * Complex(1073741827, 0))
    assert_equal(Complex(1, 1),
                 Complex(1073741827, 1073741827) / Complex(1073741827, 0))
    assert_equal(Complex(2147483616, 2147483616),
                 Complex(1073741827, 1073741827) + Complex(1073741789, 1073741789))
    assert_equal(Complex(38, 38),
                 Complex(1073741827, 1073741827) - Complex(1073741789, 1073741789))
    assert_equal(Complex(0, 2305842940494217006),
                 Complex(1073741827, 1073741827) * Complex(1073741789, 1073741789))
    assert_equal(Complex(Rational(1073741827, 1073741789), 0),
                 Complex(1073741827, 1073741827) / Complex(1073741789, 1073741789))
    assert_equal(Complex(2147483616, 2147483654),
                 Complex(1073741827, 1073741827) + Complex(1073741789, 1073741827))
    assert_equal(Complex(38, 0),
                 Complex(1073741827, 1073741827) - Complex(1073741789, 1073741827))
    assert_equal(Complex(-40802189426, 2305842981296406432),
                 Complex(1073741827, 1073741827) * Complex(1073741789, 1073741827))
    assert_equal(Complex(Rational(1152921490648203216, 1152921470247109225), Rational(-20401094713, 1152921470247109225)),
                 Complex(1073741827, 1073741827) / Complex(1073741789, 1073741827))
    assert_equal(Complex(2147483654, 2147483654),
                 Complex(1073741827, 1073741827) + Complex(1073741827, 1073741827))
    assert_equal(Complex(0, 0),
                 Complex(1073741827, 1073741827) - Complex(1073741827, 1073741827))
    assert_equal(Complex(0, 2305843022098595858),
                 Complex(1073741827, 1073741827) * Complex(1073741827, 1073741827))
    assert_equal(Complex(1, 0),
                 Complex(1073741827, 1073741827) / Complex(1073741827, 1073741827))
    assert_equal(Complex(Rational(1152921512123039718, 1073741827), Rational(1152921512123039718, 1073741827)),
                 Complex(1073741827, 1073741827) + Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(1152921509975556140, 1073741827), Rational(1152921509975556140, 1073741827)),
                 Complex(1073741827, 1073741827) - Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(0, 2147483578),
                 Complex(1073741827, 1073741827) * Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(1152921511049297929, 1073741789), 0),
                 Complex(1073741827, 1073741827) / Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(1152921471320850330, 1073741789), Rational(1152921471320850330, 1073741789)),
                 Complex(1073741827, 1073741827) + Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1152921469173366676, 1073741789), Rational(1152921469173366676, 1073741789)),
                 Complex(1073741827, 1073741827) - Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(0, Rational(2305843022098595858, 1073741789)),
                 Complex(1073741827, 1073741827) * Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(1073741789, 0),
                 Complex(1073741827, 1073741827) / Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1152921512123039718, 1073741827), Rational(1152921471320850330, 1073741789)),
                 Complex(1073741827, 1073741827) + Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1152921509975556140, 1073741827), Rational(1152921469173366676, 1073741789)),
                 Complex(1073741827, 1073741827) - Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(-81604377408, 1073741789), Rational(2305842940494218450, 1073741789)),
                 Complex(1073741827, 1073741827) * Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1427247611623053801969816945064593334552299725, 1329227916556757626754787505905300241), Rational(-50510661722949347514642033621130734624, 1329227916556757626754787505905300241)),
                 Complex(1073741827, 1073741827) / Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)), +Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(-1073741789, 1073741827), Rational(-1073741789, 1073741827)), -Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(2147483616, 1073741827), Rational(1073741789, 1073741827)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) + Complex(1, 0))
    assert_equal(Complex(Rational(-38, 1073741827), Rational(1073741789, 1073741827)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) - Complex(1, 0))
    assert_equal(Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) * Complex(1, 0))
    assert_equal(Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) / Complex(1, 0))
    assert_equal(Complex(Rational(1152921471320850292, 1073741827), Rational(1073741789, 1073741827)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) + Complex(1073741789, 0))
    assert_equal(Complex(Rational(-1152921469173366714, 1073741827), Rational(1073741789, 1073741827)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) - Complex(1073741789, 0))
    assert_equal(Complex(Rational(1152921429444920521, 1073741827), Rational(1152921429444920521, 1073741827)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) * Complex(1073741789, 0))
    assert_equal(Complex(Rational(1, 1073741827), Rational(1, 1073741827)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) / Complex(1073741789, 0))
    assert_equal(Complex(Rational(1152921512123039718, 1073741827), Rational(1073741789, 1073741827)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) + Complex(1073741827, 0))
    assert_equal(Complex(Rational(-1152921509975556140, 1073741827), Rational(1073741789, 1073741827)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) - Complex(1073741827, 0))
    assert_equal(Complex(1073741789, 1073741789),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) * Complex(1073741827, 0))
    assert_equal(Complex(Rational(1073741789, 1152921511049297929), Rational(1073741789, 1152921511049297929)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) / Complex(1073741827, 0))
    assert_equal(Complex(Rational(1152921471320850292, 1073741827), Rational(1152921471320850292, 1073741827)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) + Complex(1073741789, 1073741789))
    assert_equal(Complex(Rational(-1152921469173366714, 1073741827), Rational(-1152921469173366714, 1073741827)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) - Complex(1073741789, 1073741789))
    assert_equal(Complex(0, Rational(2305842858889841042, 1073741827)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) * Complex(1073741789, 1073741789))
    assert_equal(Complex(Rational(1, 1073741827), 0),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) / Complex(1073741789, 1073741789))
    assert_equal(Complex(Rational(1152921471320850292, 1073741827), Rational(1152921512123039718, 1073741827)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) + Complex(1073741789, 1073741827))
    assert_equal(Complex(Rational(-1152921469173366714, 1073741827), Rational(-1152921509975556140, 1073741827)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) - Complex(1073741789, 1073741827))
    assert_equal(Complex(Rational(-40802187982, 1073741827), Rational(2305842899692029024, 1073741827)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) * Complex(1073741789, 1073741827))
    assert_equal(Complex(Rational(1152921449846014512, 1237940005850657200720054075), Rational(-20401093991, 1237940005850657200720054075)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) / Complex(1073741789, 1073741827))
    assert_equal(Complex(Rational(1152921512123039718, 1073741827), Rational(1152921512123039718, 1073741827)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) + Complex(1073741827, 1073741827))
    assert_equal(Complex(Rational(-1152921509975556140, 1073741827), Rational(-1152921509975556140, 1073741827)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) - Complex(1073741827, 1073741827))
    assert_equal(Complex(0, 2147483578),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) * Complex(1073741827, 1073741827))
    assert_equal(Complex(Rational(1073741789, 1152921511049297929), 0),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) / Complex(1073741827, 1073741827))
    assert_equal(Complex(Rational(2147483578, 1073741827), Rational(2147483578, 1073741827)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) + Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(0, 0),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) - Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(0, Rational(2305842858889841042, 1152921511049297929)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) * Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(1, 0),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) / Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(2305842940494218450, 1152921470247108503), Rational(2305842940494218450, 1152921470247108503)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) + Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(-81604377408, 1152921470247108503), Rational(-81604377408, 1152921470247108503)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) - Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(0, 2),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) * Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1152921429444920521, 1152921511049297929), 0),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) / Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(2147483578, 1073741827), Rational(2305842940494218450, 1152921470247108503)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) + Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(0, Rational(-81604377408, 1152921470247108503)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) - Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(-81604377408, 1152921511049297929), Rational(2305842940494218450, 1152921511049297929)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) * Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1329227869515036572020512360130906225, 1329227916556757626754787505905300241), Rational(-47041717725097069072123994784, 1329227916556757626754787505905300241)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)) / Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)), +Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(-1073741827, 1073741789), Rational(-1073741827, 1073741789)), -Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(2147483616, 1073741789), Rational(1073741827, 1073741789)),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) + Complex(1, 0))
    assert_equal(Complex(Rational(38, 1073741789), Rational(1073741827, 1073741789)),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) - Complex(1, 0))
    assert_equal(Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) * Complex(1, 0))
    assert_equal(Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) / Complex(1, 0))
    assert_equal(Complex(Rational(1152921430518662348, 1073741789), Rational(1073741827, 1073741789)),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) + Complex(1073741789, 0))
    assert_equal(Complex(Rational(-1152921428371178694, 1073741789), Rational(1073741827, 1073741789)),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) - Complex(1073741789, 0))
    assert_equal(Complex(1073741827, 1073741827),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) * Complex(1073741789, 0))
    assert_equal(Complex(Rational(1073741827, 1152921429444920521), Rational(1073741827, 1152921429444920521)),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) / Complex(1073741789, 0))
    assert_equal(Complex(Rational(1152921471320850330, 1073741789), Rational(1073741827, 1073741789)),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) + Complex(1073741827, 0))
    assert_equal(Complex(Rational(-1152921469173366676, 1073741789), Rational(1073741827, 1073741789)),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) - Complex(1073741827, 0))
    assert_equal(Complex(Rational(1152921511049297929, 1073741789), Rational(1152921511049297929, 1073741789)),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) * Complex(1073741827, 0))
    assert_equal(Complex(Rational(1, 1073741789), Rational(1, 1073741789)),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) / Complex(1073741827, 0))
    assert_equal(Complex(Rational(1152921430518662348, 1073741789), Rational(1152921430518662348, 1073741789)),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) + Complex(1073741789, 1073741789))
    assert_equal(Complex(Rational(-1152921428371178694, 1073741789), Rational(-1152921428371178694, 1073741789)),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) - Complex(1073741789, 1073741789))
    assert_equal(Complex(0, 2147483654),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) * Complex(1073741789, 1073741789))
    assert_equal(Complex(Rational(1073741827, 1152921429444920521), 0),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) / Complex(1073741789, 1073741789))
    assert_equal(Complex(Rational(1152921430518662348, 1073741789), Rational(1152921471320850330, 1073741789)),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) + Complex(1073741789, 1073741827))
    assert_equal(Complex(Rational(-1152921428371178694, 1073741789), Rational(-1152921469173366676, 1073741789)),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) - Complex(1073741789, 1073741827))
    assert_equal(Complex(Rational(-40802189426, 1073741789), Rational(2305842981296406432, 1073741789)),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) * Complex(1073741789, 1073741827))
    assert_equal(Complex(Rational(1152921490648203216, 1237939962039641331329903525), Rational(-20401094713, 1237939962039641331329903525)),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) / Complex(1073741789, 1073741827))
    assert_equal(Complex(Rational(1152921471320850330, 1073741789), Rational(1152921471320850330, 1073741789)),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) + Complex(1073741827, 1073741827))
    assert_equal(Complex(Rational(-1152921469173366676, 1073741789), Rational(-1152921469173366676, 1073741789)),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) - Complex(1073741827, 1073741827))
    assert_equal(Complex(0, Rational(2305843022098595858, 1073741789)),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) * Complex(1073741827, 1073741827))
    assert_equal(Complex(Rational(1, 1073741789), 0),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) / Complex(1073741827, 1073741827))
    assert_equal(Complex(Rational(2305842940494218450, 1152921470247108503), Rational(2305842940494218450, 1152921470247108503)),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) + Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(81604377408, 1152921470247108503), Rational(81604377408, 1152921470247108503)),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) - Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(0, 2),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) * Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(1152921511049297929, 1152921429444920521), 0),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) / Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(2147483654, 1073741789), Rational(2147483654, 1073741789)),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) + Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(0, 0),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) - Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(0, Rational(2305843022098595858, 1152921429444920521)),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) * Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(1, 0),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) / Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(2305842940494218450, 1152921470247108503), Rational(2147483654, 1073741789)),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) + Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(81604377408, 1152921470247108503), 0),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) - Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(-81604377408, 1152921429444920521), Rational(2305842940494218450, 1152921429444920521)),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) * Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1329227963598475351851856578029295025, 1329227916556757626754787505905300241), Rational(-47041721054734275145774394016, 1329227916556757626754787505905300241)),
                 Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)) / Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)), +Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(-1073741789, 1073741827), Rational(-1073741827, 1073741789)), -Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(2147483616, 1073741827), Rational(1073741827, 1073741789)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) + Complex(1, 0))
    assert_equal(Complex(Rational(-38, 1073741827), Rational(1073741827, 1073741789)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) - Complex(1, 0))
    assert_equal(Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) * Complex(1, 0))
    assert_equal(Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) / Complex(1, 0))
    assert_equal(Complex(Rational(1152921471320850292, 1073741827), Rational(1073741827, 1073741789)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) + Complex(1073741789, 0))
    assert_equal(Complex(Rational(-1152921469173366714, 1073741827), Rational(1073741827, 1073741789)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) - Complex(1073741789, 0))
    assert_equal(Complex(Rational(1152921429444920521, 1073741827), 1073741827),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) * Complex(1073741789, 0))
    assert_equal(Complex(Rational(1, 1073741827), Rational(1073741827, 1152921429444920521)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) / Complex(1073741789, 0))
    assert_equal(Complex(Rational(1152921512123039718, 1073741827), Rational(1073741827, 1073741789)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) + Complex(1073741827, 0))
    assert_equal(Complex(Rational(-1152921509975556140, 1073741827), Rational(1073741827, 1073741789)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) - Complex(1073741827, 0))
    assert_equal(Complex(1073741789, Rational(1152921511049297929, 1073741789)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) * Complex(1073741827, 0))
    assert_equal(Complex(Rational(1073741789, 1152921511049297929), Rational(1, 1073741789)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) / Complex(1073741827, 0))
    assert_equal(Complex(Rational(1152921471320850292, 1073741827), Rational(1152921430518662348, 1073741789)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) + Complex(1073741789, 1073741789))
    assert_equal(Complex(Rational(-1152921469173366714, 1073741827), Rational(-1152921428371178694, 1073741789)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) - Complex(1073741789, 1073741789))
    assert_equal(Complex(Rational(-81604377408, 1073741827), Rational(2305842940494218450, 1073741827)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) * Complex(1073741789, 1073741789))
    assert_equal(Complex(Rational(1152921470247109225, 1237939962039640556088331867), Rational(40802188704, 1237939962039640556088331867)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) / Complex(1073741789, 1073741789))
    assert_equal(Complex(Rational(1152921471320850292, 1073741827), Rational(1152921471320850330, 1073741789)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) + Complex(1073741789, 1073741827))
    assert_equal(Complex(Rational(-1152921469173366714, 1073741827), Rational(-1152921469173366676, 1073741789)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) - Complex(1073741789, 1073741827))
    assert_equal(Complex(Rational(-131433047608170424214, 1152921470247108503), 2147483616),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) * Complex(1073741789, 1073741827))
    assert_equal(Complex(Rational(1237939983945150041266564176, 1329227916556755129526882950667240175), Rational(19, 1152921470247109225)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) / Complex(1073741789, 1073741827))
    assert_equal(Complex(Rational(1152921512123039718, 1073741827), Rational(1152921471320850330, 1073741789)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) + Complex(1073741827, 1073741827))
    assert_equal(Complex(Rational(-1152921509975556140, 1073741827), Rational(-1152921469173366676, 1073741789)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) - Complex(1073741827, 1073741827))
    assert_equal(Complex(Rational(-81604377408, 1073741789), Rational(2305842940494218450, 1073741789)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) * Complex(1073741827, 1073741827))
    assert_equal(Complex(Rational(1152921470247109225, 1237940005850656425478454981), Rational(40802188704, 1237940005850656425478454981)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) / Complex(1073741827, 1073741827))
    assert_equal(Complex(Rational(2147483578, 1073741827), Rational(2305842940494218450, 1152921470247108503)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) + Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(0, Rational(81604377408, 1152921470247108503)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) - Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(-81604377408, 1152921511049297929), Rational(2305842940494218450, 1152921511049297929)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) * Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(1152921470247109225, 1152921429444920521), Rational(40802188704, 1152921429444920521)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) / Complex(Rational(1073741789, 1073741827), Rational(1073741789, 1073741827)))
    assert_equal(Complex(Rational(2305842940494218450, 1152921470247108503), Rational(2147483654, 1073741789)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) + Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(-81604377408, 1152921470247108503), 0),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) - Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(-81604377408, 1152921429444920521), Rational(2305842940494218450, 1152921429444920521)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) * Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(1152921470247109225, 1152921511049297929), Rational(40802188704, 1152921511049297929)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) / Complex(Rational(1073741827, 1073741789), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(2147483578, 1073741827), Rational(2147483654, 1073741789)),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) + Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(0, 0),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) - Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(Rational(-188166877559662688435796777600, 1329227916556754297117581432254901009), 2),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) * Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
    assert_equal(Complex(1, 0),
                 Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)) / Complex(Rational(1073741789, 1073741827), Rational(1073741827, 1073741789)))
  end

  def test_kumi2
    assert_equal('0.0+0.0i', (+Complex(+0.0, +0.0)).to_s)
    assert_equal('-0.0-0.0i', (-Complex(+0.0, +0.0)).to_s)
    assert_equal('0.0+0.0i', (Complex(+0.0, +0.0) + Complex(+0.0, +0.0)).to_s)
    assert_equal('0.0+0.0i', (Complex(+0.0, +0.0) - Complex(+0.0, +0.0)).to_s)
    assert_equal('0.0+0.0i', (Complex(+0.0, +0.0) * Complex(+0.0, +0.0)).to_s)
    assert_equal('0.0+0.0i', (Complex(+0.0, +0.0) + Complex(-0.0, +0.0)).to_s)
    assert_equal('0.0+0.0i', (Complex(+0.0, +0.0) - Complex(-0.0, +0.0)).to_s)
    assert_equal('-0.0+0.0i', (Complex(+0.0, +0.0) * Complex(-0.0, +0.0)).to_s)
    assert_equal('0.0+0.0i', (Complex(+0.0, +0.0) + Complex(+0.0, -0.0)).to_s)
    assert_equal('0.0+0.0i', (Complex(+0.0, +0.0) - Complex(+0.0, -0.0)).to_s)
    assert_equal('0.0+0.0i', (Complex(+0.0, +0.0) * Complex(+0.0, -0.0)).to_s)
    assert_equal('0.0+0.0i', (Complex(+0.0, +0.0) + Complex(-0.0, -0.0)).to_s)
    assert_equal('0.0+0.0i', (Complex(+0.0, +0.0) - Complex(-0.0, -0.0)).to_s)
    assert_equal('0.0-0.0i', (Complex(+0.0, +0.0) * Complex(-0.0, -0.0)).to_s)
    assert_equal('-0.0+0.0i', (+Complex(-0.0, +0.0)).to_s)
    assert_equal('0.0-0.0i', (-Complex(-0.0, +0.0)).to_s)
    assert_equal('0.0+0.0i', (Complex(-0.0, +0.0) + Complex(+0.0, +0.0)).to_s)
    assert_equal('-0.0+0.0i', (Complex(-0.0, +0.0) - Complex(+0.0, +0.0)).to_s)
    assert_equal('-0.0+0.0i', (Complex(-0.0, +0.0) * Complex(+0.0, +0.0)).to_s)
    assert_equal('-0.0+0.0i', (Complex(-0.0, +0.0) + Complex(-0.0, +0.0)).to_s)
    assert_equal('0.0+0.0i', (Complex(-0.0, +0.0) - Complex(-0.0, +0.0)).to_s)
    assert_equal('0.0-0.0i', (Complex(-0.0, +0.0) * Complex(-0.0, +0.0)).to_s)
    assert_equal('0.0+0.0i', (Complex(-0.0, +0.0) + Complex(+0.0, -0.0)).to_s)
    assert_equal('-0.0+0.0i', (Complex(-0.0, +0.0) - Complex(+0.0, -0.0)).to_s)
    assert_equal('0.0+0.0i', (Complex(-0.0, +0.0) * Complex(+0.0, -0.0)).to_s)
    assert_equal('-0.0+0.0i', (Complex(-0.0, +0.0) + Complex(-0.0, -0.0)).to_s)
    assert_equal('0.0+0.0i', (Complex(-0.0, +0.0) - Complex(-0.0, -0.0)).to_s)
    assert_equal('0.0+0.0i', (Complex(-0.0, +0.0) * Complex(-0.0, -0.0)).to_s)
    assert_equal('0.0-0.0i', (+Complex(+0.0, -0.0)).to_s)
    assert_equal('-0.0+0.0i', (-Complex(+0.0, -0.0)).to_s)
    assert_equal('0.0+0.0i', (Complex(+0.0, -0.0) + Complex(+0.0, +0.0)).to_s)
    assert_equal('0.0-0.0i', (Complex(+0.0, -0.0) - Complex(+0.0, +0.0)).to_s)
    assert_equal('0.0+0.0i', (Complex(+0.0, -0.0) * Complex(+0.0, +0.0)).to_s)
    assert_equal('0.0+0.0i', (Complex(+0.0, -0.0) + Complex(-0.0, +0.0)).to_s)
    assert_equal('0.0-0.0i', (Complex(+0.0, -0.0) - Complex(-0.0, +0.0)).to_s)
    assert_equal('0.0+0.0i', (Complex(+0.0, -0.0) * Complex(-0.0, +0.0)).to_s)
    assert_equal('0.0-0.0i', (Complex(+0.0, -0.0) + Complex(+0.0, -0.0)).to_s)
    assert_equal('0.0+0.0i', (Complex(+0.0, -0.0) - Complex(+0.0, -0.0)).to_s)
    assert_equal('0.0-0.0i', (Complex(+0.0, -0.0) * Complex(+0.0, -0.0)).to_s)
    assert_equal('0.0-0.0i', (Complex(+0.0, -0.0) + Complex(-0.0, -0.0)).to_s)
    assert_equal('0.0+0.0i', (Complex(+0.0, -0.0) - Complex(-0.0, -0.0)).to_s)
    assert_equal('-0.0+0.0i', (Complex(+0.0, -0.0) * Complex(-0.0, -0.0)).to_s)
    assert_equal('-0.0-0.0i', (+Complex(-0.0, -0.0)).to_s)
    assert_equal('0.0+0.0i', (-Complex(-0.0, -0.0)).to_s)
    assert_equal('0.0+0.0i', (Complex(-0.0, -0.0) + Complex(+0.0, +0.0)).to_s)
    assert_equal('-0.0-0.0i', (Complex(-0.0, -0.0) - Complex(+0.0, +0.0)).to_s)
    assert_equal('0.0-0.0i', (Complex(-0.0, -0.0) * Complex(+0.0, +0.0)).to_s)
    assert_equal('-0.0+0.0i', (Complex(-0.0, -0.0) + Complex(-0.0, +0.0)).to_s)
    assert_equal('0.0-0.0i', (Complex(-0.0, -0.0) - Complex(-0.0, +0.0)).to_s)
    assert_equal('0.0+0.0i', (Complex(-0.0, -0.0) * Complex(-0.0, +0.0)).to_s)
    assert_equal('0.0-0.0i', (Complex(-0.0, -0.0) + Complex(+0.0, -0.0)).to_s)
    assert_equal('-0.0+0.0i', (Complex(-0.0, -0.0) - Complex(+0.0, -0.0)).to_s)
    assert_equal('-0.0+0.0i', (Complex(-0.0, -0.0) * Complex(+0.0, -0.0)).to_s)
    assert_equal('-0.0-0.0i', (Complex(-0.0, -0.0) + Complex(-0.0, -0.0)).to_s)
    assert_equal('0.0+0.0i', (Complex(-0.0, -0.0) - Complex(-0.0, -0.0)).to_s)
    assert_equal('0.0+0.0i', (Complex(-0.0, -0.0) * Complex(-0.0, -0.0)).to_s)
  end

end
