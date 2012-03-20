require 'rubygems'
require 'minitest/autorun'
require File.expand_path '../xref_test_case', __FILE__

class TestRDocContext < XrefTestCase

  def setup
    super

    @context = RDoc::Context.new
  end

  def test_initialize
    assert_empty @context.in_files
    assert_equal 'unknown', @context.name
    assert_equal '', @context.comment
    assert_equal nil, @context.parent
    assert_equal :public, @context.visibility
    assert_equal 1, @context.sections.length
    assert_equal nil, @context.temporary_section

    assert_empty @context.classes_hash
    assert_empty @context.modules_hash

    assert_empty @context.method_list
    assert_empty @context.attributes
    assert_empty @context.aliases
    assert_empty @context.requires
    assert_empty @context.includes
    assert_empty @context.constants
  end

  def test_add_alias
    as = RDoc::Alias.new nil, 'old_name', 'new_name', 'comment'

    @context.add_alias as

    assert_equal [as], @context.external_aliases
    assert_equal [as], @context.unmatched_alias_lists['#old_name']
  end

  def test_add_alias_method_attr
    top_level = RDoc::TopLevel.new 'file.rb'

    attr = RDoc::Attr.new nil, 'old_name', 'R', ''

    as = RDoc::Alias.new nil, 'old_name', 'new_name', 'comment'
    as.record_location top_level
    as.parent = @context

    @context.add_attribute attr
    @context.add_alias as

    assert_empty @context.aliases
    assert_empty @context.unmatched_alias_lists
    assert_equal %w[old_name new_name], @context.attributes.map { |m| m.name }

    new = @context.attributes.last
    assert_equal top_level, new.file
  end

  def test_add_alias_method
    top_level = RDoc::TopLevel.new 'file.rb'

    meth = RDoc::AnyMethod.new nil, 'old_name'
    meth.singleton = false

    as = RDoc::Alias.new nil, 'old_name', 'new_name', 'comment'
    as.record_location top_level
    as.parent = @context

    @context.add_method meth
    @context.add_alias as

    assert_empty @context.aliases
    assert_empty @context.unmatched_alias_lists
    assert_equal %w[old_name new_name], @context.method_list.map { |m| m.name }

    new = @context.method_list.last
    assert_equal top_level, new.file
  end

  def test_add_alias_method_singleton
    meth = RDoc::AnyMethod.new nil, 'old_name'
    meth.singleton = true

    as = RDoc::Alias.new nil, 'old_name', 'new_name', 'comment'
    as.singleton = true

    as.parent = @context

    @context.add_method meth
    @context.add_alias as

    assert_empty @context.aliases
    assert_empty @context.unmatched_alias_lists
    assert_equal %w[old_name new_name], @context.method_list.map { |m| m.name }

    assert @context.method_list.last.singleton
  end

  def test_add_class
    @c1.add_class RDoc::NormalClass, 'Klass', 'Object'

    assert_includes @c1.classes.map { |k| k.full_name }, 'C1::Klass'
    assert_includes RDoc::TopLevel.classes.map { |k| k.full_name }, 'C1::Klass'
  end

  def test_add_class_basic_object
    skip 'BasicObject is 1.9 only' unless defined?(BasicObject)

    @xref_data.add_class RDoc::NormalClass, 'BasicObject'

    basic = @xref_data.find_module_named 'BasicObject'

    assert_nil basic.superclass

    @c1.add_class RDoc::NormalClass, 'BasicObject'

    basic = @c1.find_module_named 'BasicObject'

    assert_equal 'Object', basic.superclass
  end

  def test_add_class_object
    root_class = defined?(BasicObject) ? 'BasicObject' : nil

    @xref_data.add_class RDoc::NormalClass, 'Object'

    object = @xref_data.find_module_named 'Object'

    assert_equal root_class, object.superclass

    @c1.add_class RDoc::NormalClass, 'Object'

    object = @c1.find_module_named 'Object'

    assert_equal 'Object', object.superclass.full_name
  end

  def test_add_class_singleton
    @c1.add_class RDoc::NormalClass, 'Klass', 'Object'

    assert_includes @c1.classes.map { |k| k.full_name }, 'C1::Klass'
    assert_includes RDoc::TopLevel.classes.map { |k| k.full_name }, 'C1::Klass'
  end

  def test_add_class_superclass
    @c1.add_class RDoc::NormalClass, 'Klass', 'Object'
    @c1.add_class RDoc::NormalClass, 'Klass', 'Other'
    @c1.add_class RDoc::NormalClass, 'Klass', 'Object'

    klass = @c1.find_module_named 'Klass'
    assert_equal 'Other', klass.superclass
  end

  def test_add_class_upgrade
    @c1.add_module RDoc::NormalModule, 'Klass'
    @c1.add_class RDoc::NormalClass, 'Klass', nil

    assert_includes @c1.classes.map { |k| k.full_name }, 'C1::Klass',
                    'c1 classes'
    refute_includes @c1.modules.map { |k| k.full_name }, 'C1::Klass',
                    'c1 modules'

    assert_includes RDoc::TopLevel.classes.map { |k| k.full_name }, 'C1::Klass',
                    'TopLevel classes'
    refute_includes RDoc::TopLevel.modules.map { |k| k.full_name }, 'C1::Klass',
                    'TopLevel modules'
  end

  def test_add_constant
    const = RDoc::Constant.new 'NAME', 'value', 'comment'
    @context.add_constant const

    assert_equal [const], @context.constants
  end

  def test_add_include
    incl = RDoc::Include.new 'Name', 'comment'
    @context.add_include incl

    assert_equal [incl], @context.includes
  end

  def test_add_include_twice
    incl1 = RDoc::Include.new 'Name', 'comment'
    @context.add_include incl1

    incl2 = RDoc::Include.new 'Name', 'comment'
    @context.add_include incl2

    assert_equal [incl1], @context.includes
  end

  def test_add_method
    meth = RDoc::AnyMethod.new nil, 'old_name'
    meth.visibility = nil

    @context.add_method meth

    assert_equal [meth], @context.method_list
    assert_equal :public, meth.visibility
  end

  def test_add_method_alias
    as = RDoc::Alias.new nil, 'old_name', 'new_name', 'comment'
    meth = RDoc::AnyMethod.new nil, 'old_name'

    @context.add_alias as
    refute_empty @context.external_aliases

    @context.add_method meth

    assert_empty @context.external_aliases
    assert_empty @context.unmatched_alias_lists
    assert_equal %w[old_name new_name], @context.method_list.map { |m| m.name }
  end

  def test_add_module
    @c1.add_module RDoc::NormalModule, 'Mod'

    assert_includes @c1.modules.map { |m| m.full_name }, 'C1::Mod'
  end

  def test_add_module_alias
    tl = RDoc::TopLevel.new 'file.rb'

    c3_c4 = @c2.add_module_alias @c2_c3, 'C4', tl

    c4 = @c2.find_module_named('C4')

    alias_constant = @c2.constants.first

    assert_equal c4, c3_c4
    assert_equal tl, alias_constant.file
  end

  def test_add_module_class
    k = @c1.add_class RDoc::NormalClass, 'Klass', nil
    m = @c1.add_module RDoc::NormalModule, 'Klass'

    assert_equal k, m, 'returns class'
    assert_empty @c1.modules
  end

  def test_add_require
    req = RDoc::Require.new 'require', 'comment'
    @c1.add_require req

    assert_empty @c1.requires
    assert_includes @c1.top_level.requires, req
  end

  def test_add_section
    default_section = @context.sections.first

    @context.add_section nil, '# comment'

    assert_equal 1, @context.sections.length
    assert_equal '# comment', @context.sections.first.comment

    @context.add_section nil, '# new comment'

    assert_equal 1, @context.sections.length
    assert_equal "# comment\n# ---\n# new comment",
                 @context.sections.first.comment

    @context.add_section 'other', ''

    assert_equal 2, @context.sections.length

    new_section = @context.sections.find { |section| section.title == 'other' }
    assert new_section
    assert_equal default_section, @context.current_section
  end

  def test_add_to
    incl = RDoc::Include.new 'Name', 'comment'
    arr = []
    @context.add_to arr, incl

    assert_includes arr, incl
    assert_equal @context, incl.parent
    assert_equal @context.current_section, incl.section
  end

  def test_add_to_temporary_section
    incl = RDoc::Include.new 'Name', 'comment'
    arr = []
    section = @context.add_section 'temporary', ''
    @context.temporary_section = section

    @context.add_to arr, incl

    assert_includes arr, incl
    assert_equal @context, incl.parent
    assert_equal section, incl.section
  end

  def test_add_to_no_document_self
    incl = RDoc::Include.new 'Name', 'comment'
    arr = []
    @context.document_self = false
    @context.add_to arr, incl

    refute_includes arr, incl
  end

  def test_add_to_done_documenting
    incl = RDoc::Include.new 'Name', 'comment'
    arr = []
    @context.done_documenting = true
    @context.add_to arr, incl

    refute_includes arr, incl
  end

  def test_child_name
    assert_equal 'C1::C1', @c1.child_name('C1')
  end

  def test_classes
    assert_equal %w[C2::C3], @c2.classes.map { |k| k.full_name }
    assert_equal %w[C3::H1 C3::H2], @c3.classes.map { |k| k.full_name }
  end

  def test_current_section
    default_section = @context.current_section

    new_section = @context.add_section 'other', ''
    @context.temporary_section = new_section

    assert_equal new_section, @context.current_section
    assert_equal default_section, @context.current_section
  end

  def test_defined_in_eh
    assert @c1.defined_in?(@c1.top_level)

    refute @c1.defined_in?(RDoc::TopLevel.new('name.rb'))
  end

  def test_equals2
    assert_equal @c3,    @c3
    refute_equal @c2,    @c3
    refute_equal @c2_c3, @c3
  end

  def test_each_section
    sects  = []
    consts = []
    attrs  = []

    @c1.each_section do |section, constants, attributes|
      sects  << section
      consts << constants
      attrs  << attributes
    end

    assert_equal [nil, 'separate'], sects.map { |section| section.title }

    expected_consts = [
      [@c1.constants.first],
      [],
    ]

    assert_equal expected_consts, consts

    expected_attrs = [
      [@c1.attributes[0], @c1.attributes[3]],
      [@c1.attributes[1], @c1.attributes[2]],
    ]

    assert_equal expected_attrs, attrs
  end

  def test_find_attribute_named
    assert_equal nil,  @c1.find_attribute_named('none')
    assert_equal 'R',  @c1.find_attribute_named('attr').rw
    assert_equal 'R',  @c1.find_attribute_named('attr_reader').rw
    assert_equal 'W',  @c1.find_attribute_named('attr_writer').rw
    assert_equal 'RW', @c1.find_attribute_named('attr_accessor').rw
  end

  def test_find_class_method_named
    assert_equal nil, @c1.find_class_method_named('none')

    m = @c1.find_class_method_named('m')
    assert_instance_of RDoc::AnyMethod, m
    assert m.singleton
  end

  def test_find_constant_named
    assert_equal nil,      @c1.find_constant_named('NONE')
    assert_equal ':const', @c1.find_constant_named('CONST').value
  end

  def test_find_enclosing_module_named
    assert_equal nil, @c2_c3.find_enclosing_module_named('NONE')
    assert_equal @c1, @c2_c3.find_enclosing_module_named('C1')
    assert_equal @c2, @c2_c3.find_enclosing_module_named('C2')
  end

  def test_find_file_named
    assert_equal nil,        @c1.find_file_named('nonexistent.rb')
    assert_equal @xref_data, @c1.find_file_named(@file_name)
  end

  def test_find_instance_method_named
    assert_equal nil, @c1.find_instance_method_named('none')

    m = @c1.find_instance_method_named('m')
    assert_instance_of RDoc::AnyMethod, m
    refute m.singleton
  end

  def test_find_local_symbol
    assert_equal true,       @c1.find_local_symbol('m').singleton
    assert_equal ':const',   @c1.find_local_symbol('CONST').value
    assert_equal 'R',        @c1.find_local_symbol('attr').rw
    assert_equal @xref_data, @c1.find_local_symbol(@file_name)
    assert_equal @c2_c3,     @c2.find_local_symbol('C3')
  end

  def test_find_method_named
    assert_equal true, @c1.find_method_named('m').singleton
  end

  def test_find_module_named
    assert_equal @c2_c3, @c2.find_module_named('C3')
    assert_equal @c2,    @c2.find_module_named('C2')
    assert_equal @c1,    @c2.find_module_named('C1')

    assert_equal 'C2::C3', @c2.find_module_named('C3').full_name
  end

  def test_find_symbol
    c3 = @xref_data.find_module_named('C3')
    assert_equal c3,     @xref_data.find_symbol('C3')
    assert_equal c3,     @c2.find_symbol('::C3')
    assert_equal @c2_c3, @c2.find_symbol('C3')
  end

  def test_find_symbol_method
    assert_equal @c1__m, @c1.find_symbol('m')
    assert_equal @c1_m,  @c1.find_symbol('#m')
    assert_equal @c1__m, @c1.find_symbol('::m')
  end

  def test_fully_documented_eh
    context = RDoc::Context.new

    refute context.fully_documented?

    context.comment = 'hi'

    assert context.fully_documented?

    m = @c1_m

    context.add_method m

    refute context.fully_documented?

    m.comment = 'hi'

    assert context.fully_documented?

    c = RDoc::Constant.new 'C', '0', nil

    context.add_constant c

    refute context.fully_documented?

    c.comment = 'hi'

    assert context.fully_documented?

    a = RDoc::Attr.new '', 'a', 'RW', nil

    context.add_attribute a

    refute context.fully_documented?

    a.comment = 'hi'

    assert context.fully_documented?
  end

  def test_spaceship
    assert_equal(-1, @c2.<=>(@c3))
    assert_equal 0,  @c2.<=>(@c2)
    assert_equal 1,  @c3.<=>(@c2)

    assert_equal 1,  @c2_c3.<=>(@c2)
    assert_equal(-1, @c2_c3.<=>(@c3))
  end

  def test_methods_by_type
    expected = {
      'instance' => {
        :private   => [],
        :protected => [],
        :public    => [@c1_m],
      },
      'class' => {
        :private   => [],
        :protected => [],
        :public    => [@c1__m],
      },
    }

    assert_equal expected, @c1.methods_by_type
  end

  def test_methods_by_type_section
    separate = @c1.sections_hash['separate']
    @c1_m.section = separate

    expected = {
      'instance' => {
        :private   => [],
        :protected => [],
        :public    => [@c1_m],
      },
      'class' => {
        :private   => [],
        :protected => [],
        :public    => [],
      },
    }

    assert_equal expected, @c1.methods_by_type(separate)
  end

  def test_methods_matching
    methods = []

    @parent.methods_matching 'm' do |m|
      methods << m
    end

    assert_equal [@parent_m], methods
  end

  def test_methods_matching_singleton
    methods = []

    @parent.methods_matching 'm', true do |m|
      methods << m
    end

    assert_equal [@parent__m], methods
  end

  def test_methods_matching_inherit
    methods = []

    @child.methods_matching 'm' do |m|
      methods << m
    end

    assert_equal [@parent_m], methods
  end

  def test_remove_invisible_private
    util_visibilities

    @vis.remove_invisible :private

    assert_equal [@pub, @prot, @priv], @vis.method_list
    assert_equal [@apub, @aprot, @apriv], @vis.attributes
  end

  def test_remove_invisible_protected
    util_visibilities

    @vis.remove_invisible :protected

    assert_equal [@pub, @prot], @vis.method_list
    assert_equal [@apub, @aprot], @vis.attributes
  end

  def test_remove_invisible_public
    util_visibilities

    @vis.remove_invisible :public

    assert_equal [@pub], @vis.method_list
    assert_equal [@apub], @vis.attributes
  end

  def test_remove_invisible_public_force
    util_visibilities

    @priv.force_documentation = true
    @prot.force_documentation = true
    @apriv.force_documentation = true
    @aprot.force_documentation = true

    @vis.remove_invisible :public

    assert_equal [@pub, @prot, @priv], @vis.method_list
    assert_equal [@apub, @aprot, @apriv], @vis.attributes
  end

  def test_remove_invisible_in_protected
    util_visibilities

    methods = [@pub, @prot, @priv]

    @c1.remove_invisible_in methods, :protected

    assert_equal [@pub, @prot], methods
  end

  def test_remove_invisible_in_protected_force
    util_visibilities

    @priv.force_documentation = true

    methods = [@pub, @prot, @priv]

    @c1.remove_invisible_in methods, :protected

    assert_equal [@pub, @prot, @priv], methods
  end

  def test_remove_invisible_in_public
    util_visibilities

    methods = [@pub, @prot, @priv]

    @c1.remove_invisible_in methods, :public

    assert_equal [@pub], methods
  end

  def test_remove_invisible_in_public_force
    util_visibilities

    @prot.force_documentation = true
    @priv.force_documentation = true

    methods = [@pub, @prot, @priv]

    @c1.remove_invisible_in methods, :public

    assert_equal [@pub, @prot, @priv], methods
  end

  def test_set_current_section
    default_section = @context.sections.first

    @context.set_current_section nil, ''

    assert_equal default_section, @context.current_section

    @context.set_current_section 'other', ''

    new_section = @context.sections.find { |section|
      section != default_section
    }

    assert_equal new_section, @context.current_section
  end

  def util_visibilities
    @pub  = RDoc::AnyMethod.new nil, 'pub'
    @prot = RDoc::AnyMethod.new nil, 'prot'
    @priv = RDoc::AnyMethod.new nil, 'priv'

    @apub  = RDoc::Attr.new nil, 'pub',  'RW', nil
    @aprot = RDoc::Attr.new nil, 'prot', 'RW', nil
    @apriv = RDoc::Attr.new nil, 'priv', 'RW', nil

    @vis = RDoc::NormalClass.new 'Vis'
    @vis.add_method @pub
    @vis.add_method @prot
    @vis.add_method @priv

    @vis.add_attribute @apub
    @vis.add_attribute @aprot
    @vis.add_attribute @apriv

    @prot.visibility = :protected
    @priv.visibility = :private

    @aprot.visibility = :protected
    @apriv.visibility = :private
  end

end

