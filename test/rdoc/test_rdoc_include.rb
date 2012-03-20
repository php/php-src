require File.expand_path '../xref_test_case', __FILE__

class TestRDocInclude < XrefTestCase

  def setup
    super

    @inc = RDoc::Include.new 'M1', 'comment'
    @inc.parent = @m1
  end

  def test_module
    assert_equal @m1, @inc.module
    assert_equal 'Unknown', RDoc::Include.new('Unknown', 'comment').module
  end

  def test_module_extended
    m1 = @xref_data.add_module RDoc::NormalModule, 'Mod1'
    m1_m3 = m1.add_module RDoc::NormalModule, 'Mod3'
    m1_m2 = m1.add_module RDoc::NormalModule, 'Mod2'
    m1_m2_m3 = m1_m2.add_module RDoc::NormalModule, 'Mod3'
    m1_m2_m3_m4 = m1_m2_m3.add_module RDoc::NormalModule, 'Mod4'
    m1_m2_m4 = m1_m2.add_module RDoc::NormalModule, 'Mod4'
    m1_m2_k0 = m1_m2.add_class RDoc::NormalClass, 'Klass0'
    m1_m2_k0_m4 = m1_m2_k0.add_module RDoc::NormalModule, 'Mod4'
    #m1_m2_k0_m4_m5 = m1_m2_k0_m4.add_module RDoc::NormalModule, 'Mod5'
    m1_m2_k0_m4_m6 = m1_m2_k0_m4.add_module RDoc::NormalModule, 'Mod6'
    m1_m2_k0_m5 = m1_m2_k0.add_module RDoc::NormalModule, 'Mod5'

    i0_m4 = RDoc::Include.new 'Mod4', nil
    i0_m5 = RDoc::Include.new 'Mod5', nil
    i0_m6 = RDoc::Include.new 'Mod6', nil
    i0_m1 = RDoc::Include.new 'Mod1', nil
    i0_m2 = RDoc::Include.new 'Mod2', nil
    i0_m3 = RDoc::Include.new 'Mod3', nil

    m1_m2_k0.add_include i0_m4
    m1_m2_k0.add_include i0_m5
    m1_m2_k0.add_include i0_m6
    m1_m2_k0.add_include i0_m1
    m1_m2_k0.add_include i0_m2
    m1_m2_k0.add_include i0_m3

    assert_equal [i0_m4, i0_m5, i0_m6, i0_m1, i0_m2, i0_m3], m1_m2_k0.includes
    assert_equal [m1_m2_m3, m1_m2, m1, m1_m2_k0_m4_m6, m1_m2_k0_m5,
                  m1_m2_k0_m4, 'Object'], m1_m2_k0.ancestors

    m1_k1 = m1.add_class RDoc::NormalClass, 'Klass1'

    i1_m1 = RDoc::Include.new 'Mod1', nil
    i1_m2 = RDoc::Include.new 'Mod2', nil
    i1_m3 = RDoc::Include.new 'Mod3', nil
    i1_m4 = RDoc::Include.new 'Mod4', nil
    i1_k0_m4 = RDoc::Include.new 'Klass0::Mod4', nil

    m1_k1.add_include i1_m1
    m1_k1.add_include i1_m2
    m1_k1.add_include i1_m3
    m1_k1.add_include i1_m4
    m1_k1.add_include i1_k0_m4

    assert_equal [i1_m1, i1_m2, i1_m3, i1_m4, i1_k0_m4], m1_k1.includes
    assert_equal [m1_m2_k0_m4, m1_m2_m3_m4, m1_m2_m3, m1_m2, m1, 'Object'],
                 m1_k1.ancestors

    m1_k2 = m1.add_class RDoc::NormalClass, 'Klass2'

    i2_m1 = RDoc::Include.new 'Mod1', nil
    i2_m2 = RDoc::Include.new 'Mod2', nil
    i2_m3 = RDoc::Include.new 'Mod3', nil
    i2_k0_m4 = RDoc::Include.new 'Klass0::Mod4', nil

    m1_k2.add_include i2_m1
    m1_k2.add_include i2_m3
    m1_k2.add_include i2_m2
    m1_k2.add_include i2_k0_m4

    assert_equal [i2_m1, i2_m3, i2_m2, i2_k0_m4], m1_k2.includes
    assert_equal [m1_m2_k0_m4, m1_m2, m1_m3, m1, 'Object'], m1_k2.ancestors

    m1_k3 = m1.add_class RDoc::NormalClass, 'Klass3'

    i3_m1 = RDoc::Include.new 'Mod1', nil
    i3_m2 = RDoc::Include.new 'Mod2', nil
    i3_m4 = RDoc::Include.new 'Mod4', nil

    m1_k3.add_include i3_m1
    m1_k3.add_include i3_m2
    m1_k3.add_include i3_m4

    assert_equal [i3_m1, i3_m2, i3_m4], m1_k3.includes
    assert_equal [m1_m2_m4, m1_m2, m1, 'Object'], m1_k3.ancestors
  end

end

