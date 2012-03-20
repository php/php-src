require File.expand_path '../xref_test_case', __FILE__

class TestRDocMethodAttr < XrefTestCase

  def test_block_params_equal

    m = RDoc::MethodAttr.new(nil, 'foo')

    m.block_params = ''
    assert_equal '', m.block_params

    m.block_params = 'a_var'
    assert_equal 'a_var', m.block_params

    m.block_params = '()'
    assert_equal '', m.block_params

    m.block_params = '(a_var, b_var)'
    assert_equal 'a_var, b_var', m.block_params

    m.block_params = '.to_s + "</#{element.upcase}>"'
    assert_equal '', m.block_params

    m.block_params = 'i.name'
    assert_equal 'name', m.block_params

    m.block_params = 'attr.expanded_name, attr.value'
    assert_equal 'expanded_name, value', m.block_params

    m.block_params = 'expanded_name, attr.value'
    assert_equal 'expanded_name, value', m.block_params

    m.block_params = 'attr.expanded_name, value'
    assert_equal 'expanded_name, value', m.block_params

    m.block_params = '(@base_notifier)'
    assert_equal 'base_notifier', m.block_params

    m.block_params = 'if @signal_status == :IN_LOAD'
    assert_equal '', m.block_params

    m.block_params = 'e if e.kind_of? Element'
    assert_equal 'e', m.block_params

    m.block_params = '(e, f) if e.kind_of? Element'
    assert_equal 'e, f', m.block_params

    m.block_params = 'back_path, back_name'
    assert_equal 'back_path, back_name', m.block_params

    m.block_params = '(*a[1..-1])'
    assert_equal '*a', m.block_params

    m.block_params = '@@context[:node] if defined? @@context[:node].namespace'
    assert_equal 'context', m.block_params

    m.block_params = '(result, klass.const_get(constant_name))'
    assert_equal 'result, const', m.block_params

    m.block_params = 'name.to_s if (bitmap & bit) != 0'
    assert_equal 'name', m.block_params

    m.block_params = 'line unless line.deleted'
    assert_equal 'line', m.block_params

    m.block_params = 'str + rs'
    assert_equal 'str', m.block_params

    m.block_params = 'f+rs'
    assert_equal 'f', m.block_params

    m.block_params = '[user, realm, hash[user]]'
    assert_equal 'user, realm, hash', m.block_params

    m.block_params = 'proc{|rc| rc == "rc" ? irbrc : irbrc+rc| ... }'
    assert_equal 'proc', m.block_params

    m.block_params = 'lambda { |x| x.to_i }'
    assert_equal 'lambda', m.block_params

    m.block_params = '$&'
    assert_equal 'str', m.block_params

    m.block_params = 'Inflections.instance'
    assert_equal 'instance', m.block_params

    m.block_params = 'self.class::STARTED'
    assert_equal 'STARTED', m.block_params

    m.block_params = 'Test::Unit::TestCase::STARTED'
    assert_equal 'STARTED', m.block_params

    m.block_params = 'ActiveSupport::OptionMerger.new(self, options)'
    assert_equal 'option_merger', m.block_params

    m.block_params = ', msg'
    assert_equal '', m.block_params

    m.block_params = '[size.to_s(16), term, chunk, term].join'
    assert_equal '[size, term, chunk, term].join', m.block_params

    m.block_params = 'YPath.new( path )'
    assert_equal 'y_path', m.block_params

  end

  def test_find_method_or_attribute_recursive
    inc = RDoc::Include.new 'M1', nil
    @m1.add_include inc # M1 now includes itself

    assert_nil @m1_m.find_method_or_attribute 'm'
  end

  def test_to_s
    assert_equal 'RDoc::AnyMethod: C1#m',  @c1_m.to_s
    assert_equal 'RDoc::AnyMethod: C2#b',  @c2_b.to_s
    assert_equal 'RDoc::AnyMethod: C1::m', @c1__m.to_s
  end


end

