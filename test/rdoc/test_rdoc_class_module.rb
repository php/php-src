require 'pp'
require File.expand_path '../xref_test_case', __FILE__

class TestRDocClassModule < XrefTestCase

  def setup
    super

    @RM = RDoc::Markup
  end

  def mu_pp obj
    s = ''
    s = PP.pp obj, s
    s.force_encoding Encoding.default_external if defined? Encoding
    s.chomp
  end

  def test_add_comment
    tl1 = RDoc::TopLevel.new 'one.rb'
    tl2 = RDoc::TopLevel.new 'two.rb'
    tl3 = RDoc::TopLevel.new 'three.rb'

    cm = RDoc::ClassModule.new 'Klass'
    cm.add_comment '# comment 1', tl1

    assert_equal [['comment 1', tl1]], cm.comment_location
    assert_equal 'comment 1', cm.comment

    cm.add_comment '# comment 2', tl2

    assert_equal [['comment 1', tl1], ['comment 2', tl2]], cm.comment_location
    assert_equal "comment 1\n---\ncomment 2", cm.comment

    cm.add_comment "# * comment 3", tl3

    assert_equal [['comment 1', tl1],
                  ['comment 2', tl2],
                  ['* comment 3', tl3]], cm.comment_location
    assert_equal "comment 1\n---\ncomment 2\n---\n* comment 3", cm.comment
  end

  def test_add_comment_stopdoc
    tl = RDoc::TopLevel.new 'file.rb'

    cm = RDoc::ClassModule.new 'Klass'
    cm.stop_doc

    cm.add_comment '# comment 1', tl

    assert_empty cm.comment
  end

  def test_ancestors
    assert_equal [@parent], @child.ancestors
  end

  def test_comment_equals
    cm = RDoc::ClassModule.new 'Klass'
    cm.comment = '# comment 1'

    assert_equal 'comment 1', cm.comment

    cm.comment = '# comment 2'

    assert_equal "comment 1\n---\ncomment 2", cm.comment

    cm.comment = "# * comment 3"

    assert_equal "comment 1\n---\ncomment 2\n---\n* comment 3", cm.comment
  end

  def test_each_ancestor
    ancestors = []

    @child.each_ancestor do |mod|
      ancestors << mod
    end

    assert_equal [@parent], ancestors
  end

  # handle making a short module alias of yourself

  def test_find_class_named
    @c2.classes_hash['C2'] = @c2

    assert_nil @c2.find_class_named('C1')
  end

  def test_from_module_comment
    tl = RDoc::TopLevel.new 'file.rb'
    klass = tl.add_class RDoc::NormalModule, 'Klass'
    klass.add_comment 'really a class', tl

    klass = RDoc::ClassModule.from_module RDoc::NormalClass, klass

    assert_equal [['really a class', tl]], klass.comment_location
  end

  def test_marshal_dump
    tl = RDoc::TopLevel.new 'file.rb'

    ns = tl.add_module RDoc::NormalModule, 'Namespace'

    cm = ns.add_class RDoc::NormalClass, 'Klass', 'Super'
    cm.record_location tl

    a1 = RDoc::Attr.new nil, 'a1', 'RW', ''
    a1.record_location tl
    a2 = RDoc::Attr.new nil, 'a2', 'RW', '', true
    a2.record_location tl

    m1 = RDoc::AnyMethod.new nil, 'm1'
    m1.record_location tl

    c1 = RDoc::Constant.new 'C1', nil, ''
    c1.record_location tl

    i1 = RDoc::Include.new 'I1', ''
    i1.record_location tl

    cm.add_attribute a1
    cm.add_attribute a2
    cm.add_method m1
    cm.add_constant c1
    cm.add_include i1
    cm.add_comment 'this is a comment', tl

    loaded = Marshal.load Marshal.dump cm

    assert_equal cm, loaded

    inner = RDoc::Markup::Document.new(
      RDoc::Markup::Paragraph.new('this is a comment'))
    inner.file = tl.absolute_name

    comment = RDoc::Markup::Document.new inner

    assert_equal [a2, a1],           loaded.attributes.sort
    assert_equal comment,            loaded.comment
    assert_equal [c1],               loaded.constants
    assert_equal 'Namespace::Klass', loaded.full_name
    assert_equal [i1],               loaded.includes
    assert_equal [m1],               loaded.method_list
    assert_equal 'Klass',            loaded.name
    assert_equal 'Super',            loaded.superclass

    assert_equal tl, loaded.attributes.first.file

    assert_equal tl, loaded.constants.first.file

    assert_equal tl, loaded.includes.first.file

    assert_equal tl, loaded.method_list.first.file
  end

  def test_marshal_load_version_0
    tl = RDoc::TopLevel.new 'file.rb'
    ns = tl.add_module RDoc::NormalModule, 'Namespace'
    cm = ns.add_class RDoc::NormalClass, 'Klass', 'Super'

    a = RDoc::Attr.new(nil, 'a1', 'RW', '')
    m = RDoc::AnyMethod.new(nil, 'm1')
    c = RDoc::Constant.new('C1', nil, '')
    i = RDoc::Include.new('I1', '')

    cm.add_attribute a
    cm.add_method m
    cm.add_constant c
    cm.add_include i
    cm.add_comment 'this is a comment', tl

    loaded = Marshal.load "\x04\bU:\x16RDoc::NormalClass[\x0Ei\x00\"\nKlass" \
                          "\"\x15Namespace::KlassI\"\nSuper\x06:\x06EF" \
                          "o:\eRDoc::Markup::Document\x06:\v@parts[\x06" \
                          "o:\x1CRDoc::Markup::Paragraph\x06;\b[\x06I" \
                          "\"\x16this is a comment\x06;\x06F[\x06[\aI" \
                          "\"\aa1\x06;\x06FI\"\aRW\x06;\x06F[\x06[\aI" \
                          "\"\aC1\x06;\x06Fo;\a\x06;\b[\x00[\x06[\aI" \
                          "\"\aI1\x06;\x06Fo;\a\x06;\b[\x00[\a[\aI" \
                          "\"\nclass\x06;\x06F[\b[\a:\vpublic[\x00[\a" \
                          ":\x0Eprotected[\x00[\a:\fprivate[\x00[\aI" \
                          "\"\rinstance\x06;\x06F[\b[\a;\n[\x06I" \
                          "\"\am1\x06;\x06F[\a;\v[\x00[\a;\f[\x00"

    assert_equal cm, loaded

    comment = RDoc::Markup::Document.new(
                RDoc::Markup::Paragraph.new('this is a comment'))

    assert_equal [a],                loaded.attributes
    assert_equal comment,            loaded.comment
    assert_equal [c],                loaded.constants
    assert_equal 'Namespace::Klass', loaded.full_name
    assert_equal [i],                loaded.includes
    assert_equal [m],                loaded.method_list
    assert_equal 'Klass',            loaded.name
    assert_equal 'Super',            loaded.superclass
    assert_equal nil,                loaded.file
  end

  def test_merge_attributes
    tl1 = RDoc::TopLevel.new 'one.rb'
    tl2 = RDoc::TopLevel.new 'two.rb'

    cm1 = RDoc::ClassModule.new 'Klass'

    attr = cm1.add_attribute RDoc::Attr.new(nil, 'a1', 'RW', '')
    attr.record_location tl1
    attr = cm1.add_attribute RDoc::Attr.new(nil, 'a3', 'R', '')
    attr.record_location tl1
    attr = cm1.add_attribute RDoc::Attr.new(nil, 'a4', 'R', '')
    attr.record_location tl1

    cm2 = RDoc::ClassModule.new 'Klass'
    # TODO allow merging when comment == ''
    cm2.instance_variable_set :@comment, @RM::Document.new

    attr = cm2.add_attribute RDoc::Attr.new(nil, 'a2', 'RW', '')
    attr.record_location tl2
    attr = cm2.add_attribute RDoc::Attr.new(nil, 'a3', 'W', '')
    attr.record_location tl1
    attr = cm2.add_attribute RDoc::Attr.new(nil, 'a4', 'W', '')
    attr.record_location tl1

    cm1.merge cm2

    expected = [
      RDoc::Attr.new(nil, 'a2', 'RW', ''),
      RDoc::Attr.new(nil, 'a3', 'W',  ''),
      RDoc::Attr.new(nil, 'a4', 'W',  ''),
    ]

    expected.each do |a| a.parent = cm1 end
    assert_equal expected, cm1.attributes.sort
  end

  def test_merge_attributes_version_0
    tl1 = RDoc::TopLevel.new 'one.rb'

    cm1 = RDoc::ClassModule.new 'Klass'

    attr = cm1.add_attribute RDoc::Attr.new(nil, 'a1', 'RW', '')
    attr.record_location tl1
    attr = cm1.add_attribute RDoc::Attr.new(nil, 'a3', 'R', '')
    attr.record_location tl1
    attr = cm1.add_attribute RDoc::Attr.new(nil, 'a4', 'R', '')
    attr.record_location tl1

    cm2 = RDoc::ClassModule.new 'Klass'
    # TODO allow merging when comment == ''
    cm2.instance_variable_set :@comment, @RM::Document.new

    attr = cm2.add_attribute RDoc::Attr.new(nil, 'a2', 'RW', '')
    attr = cm2.add_attribute RDoc::Attr.new(nil, 'a3', 'W', '')
    attr = cm2.add_attribute RDoc::Attr.new(nil, 'a4', 'W', '')

    cm1.merge cm2

    expected = [
      RDoc::Attr.new(nil, 'a1', 'RW', ''),
      RDoc::Attr.new(nil, 'a2', 'RW', ''),
      RDoc::Attr.new(nil, 'a3', 'RW', ''),
      RDoc::Attr.new(nil, 'a4', 'RW', ''),
    ]

    expected.each do |a| a.parent = cm1 end
    assert_equal expected, cm1.attributes.sort
  end

  def test_merge_collections_drop
    tl = RDoc::TopLevel.new 'file'

    cm1 = RDoc::ClassModule.new 'C'
    cm1.record_location tl

    const = cm1.add_constant RDoc::Constant.new('CONST', nil, nil)
    const.record_location tl

    cm2 = RDoc::ClassModule.new 'C'
    cm2.record_location tl

    added = []
    removed = []

    cm1.merge_collections cm1.constants, cm2.constants, cm2.in_files do |add, c|
      if add then
        added << c
      else
        removed << c
      end
    end

    assert_empty added
    assert_equal [const], removed
  end

  def test_merge_comment
    tl1 = RDoc::TopLevel.new 'one.rb'
    tl2 = RDoc::TopLevel.new 'two.rb'

    cm1 = RDoc::ClassModule.new 'Klass'
    cm1.add_comment 'klass 1', tl1

    cm2 = RDoc::ClassModule.new 'Klass'
    cm2.add_comment 'klass 2', tl2
    cm2.add_comment 'klass 3', tl1

    cm2 = Marshal.load Marshal.dump cm2

    cm1.merge cm2

    inner1 = @RM::Document.new @RM::Paragraph.new 'klass 3'
    inner1.file = 'one.rb'
    inner2 = @RM::Document.new @RM::Paragraph.new 'klass 2'
    inner2.file = 'two.rb'

    expected = @RM::Document.new inner2, inner1

    assert_equal expected, cm1.comment
  end

  def test_merge_comment_version_0
    tl = RDoc::TopLevel.new 'file.rb'

    cm1 = RDoc::ClassModule.new 'Klass'
    cm1.add_comment 'klass 1', tl

    cm2 = RDoc::ClassModule.new 'Klass'

    cm2.instance_variable_set(:@comment,
                              @RM::Document.new(
                                @RM::Paragraph.new('klass 2')))
    cm2.instance_variable_set :@comment_location, @RM::Document.new(cm2.comment)

    cm1.merge cm2

    inner = @RM::Document.new @RM::Paragraph.new 'klass 1'
    inner.file = 'file.rb'

    expected = @RM::Document.new \
      inner,
      @RM::Document.new(@RM::Paragraph.new('klass 2'))

    assert_equal expected, cm1.comment
  end

  def test_merge_constants
    tl1 = RDoc::TopLevel.new 'one.rb'
    tl2 = RDoc::TopLevel.new 'two.rb'

    cm1 = RDoc::ClassModule.new 'Klass'

    const = cm1.add_constant RDoc::Constant.new('C1', nil, 'one')
    const.record_location tl1
    const = cm1.add_constant RDoc::Constant.new('C3', nil, 'one')
    const.record_location tl1

    cm2 = RDoc::ClassModule.new 'Klass'
    cm2.instance_variable_set :@comment, @RM::Document.new

    const = cm2.add_constant RDoc::Constant.new('C2', nil, 'two')
    const.record_location tl2
    const = cm2.add_constant RDoc::Constant.new('C3', nil, 'one')
    const.record_location tl1
    const = cm2.add_constant RDoc::Constant.new('C4', nil, 'one')
    const.record_location tl1

    cm1.merge cm2

    expected = [
      RDoc::Constant.new('C2', nil, 'two'),
      RDoc::Constant.new('C3', nil, 'one'),
      RDoc::Constant.new('C4', nil, 'one'),
    ]

    expected.each do |a| a.parent = cm1 end

    assert_equal expected, cm1.constants.sort
  end

  def test_merge_constants_version_0
    tl1 = RDoc::TopLevel.new 'one.rb'

    cm1 = RDoc::ClassModule.new 'Klass'

    const = cm1.add_constant RDoc::Constant.new('C1', nil, 'one')
    const.record_location tl1
    const = cm1.add_constant RDoc::Constant.new('C3', nil, 'one')
    const.record_location tl1

    cm2 = RDoc::ClassModule.new 'Klass'
    cm2.instance_variable_set :@comment, @RM::Document.new

    const = cm2.add_constant RDoc::Constant.new('C2', nil, 'two')
    const = cm2.add_constant RDoc::Constant.new('C3', nil, 'two')
    const = cm2.add_constant RDoc::Constant.new('C4', nil, 'two')

    cm1.merge cm2

    expected = [
      RDoc::Constant.new('C1', nil, 'one'),
      RDoc::Constant.new('C2', nil, 'two'),
      RDoc::Constant.new('C3', nil, 'one'),
      RDoc::Constant.new('C4', nil, 'two'),
    ]

    expected.each do |a| a.parent = cm1 end

    assert_equal expected, cm1.constants.sort
  end

  def test_merge_includes
    tl1 = RDoc::TopLevel.new 'one.rb'
    tl2 = RDoc::TopLevel.new 'two.rb'

    cm1 = RDoc::ClassModule.new 'Klass'

    incl = cm1.add_include RDoc::Include.new('I1', 'one')
    incl.record_location tl1
    incl = cm1.add_include RDoc::Include.new('I3', 'one')
    incl.record_location tl1

    cm2 = RDoc::ClassModule.new 'Klass'
    cm2.instance_variable_set :@comment, @RM::Document.new

    incl = cm2.add_include RDoc::Include.new('I2', 'two')
    incl.record_location tl2
    incl = cm2.add_include RDoc::Include.new('I3', 'one')
    incl.record_location tl1
    incl = cm2.add_include RDoc::Include.new('I4', 'one')
    incl.record_location tl1

    cm1.merge cm2

    expected = [
      RDoc::Include.new('I2', 'two'),
      RDoc::Include.new('I3', 'one'),
      RDoc::Include.new('I4', 'one'),
    ]

    expected.each do |a| a.parent = cm1 end

    assert_equal expected, cm1.includes.sort
  end

  def test_merge_includes_version_0
    tl1 = RDoc::TopLevel.new 'one.rb'

    cm1 = RDoc::ClassModule.new 'Klass'

    incl = cm1.add_include RDoc::Include.new('I1', 'one')
    incl.record_location tl1
    incl = cm1.add_include RDoc::Include.new('I3', 'one')
    incl.record_location tl1

    cm2 = RDoc::ClassModule.new 'Klass'
    cm2.instance_variable_set :@comment, @RM::Document.new

    incl = cm2.add_include RDoc::Include.new('I2', 'two')
    incl = cm2.add_include RDoc::Include.new('I3', 'two')
    incl = cm2.add_include RDoc::Include.new('I4', 'two')

    cm1.merge cm2

    expected = [
      RDoc::Include.new('I1', 'one'),
      RDoc::Include.new('I2', 'two'),
      RDoc::Include.new('I3', 'one'),
      RDoc::Include.new('I4', 'two'),
    ]

    expected.each do |a| a.parent = cm1 end

    assert_equal expected, cm1.includes.sort
  end

  def test_merge_methods
    tl1 = RDoc::TopLevel.new 'one.rb'
    tl2 = RDoc::TopLevel.new 'two.rb'

    cm1 = RDoc::ClassModule.new 'Klass'

    meth = cm1.add_method RDoc::AnyMethod.new(nil, 'm1')
    meth.record_location tl1
    meth = cm1.add_method RDoc::AnyMethod.new(nil, 'm3')
    meth.record_location tl1

    cm2 = RDoc::ClassModule.new 'Klass'
    cm2.instance_variable_set :@comment, @RM::Document.new

    meth = cm2.add_method RDoc::AnyMethod.new(nil, 'm2')
    meth.record_location tl2
    meth = cm2.add_method RDoc::AnyMethod.new(nil, 'm3')
    meth.record_location tl1
    meth = cm2.add_method RDoc::AnyMethod.new(nil, 'm4')
    meth.record_location tl1

    cm1.merge cm2

    expected = [
      RDoc::AnyMethod.new(nil, 'm2'),
      RDoc::AnyMethod.new(nil, 'm3'),
      RDoc::AnyMethod.new(nil, 'm4'),
    ]

    expected.each do |a| a.parent = cm1 end

    assert_equal expected, cm1.method_list.sort
  end

  def test_merge_methods_version_0
    tl1 = RDoc::TopLevel.new 'one.rb'

    cm1 = RDoc::ClassModule.new 'Klass'

    meth = cm1.add_method RDoc::AnyMethod.new(nil, 'm1')
    meth.record_location tl1
    meth = cm1.add_method RDoc::AnyMethod.new(nil, 'm3')
    meth.record_location tl1

    cm2 = RDoc::ClassModule.new 'Klass'
    cm2.instance_variable_set :@comment, @RM::Document.new

    meth = cm2.add_method RDoc::AnyMethod.new(nil, 'm2')
    meth = cm2.add_method RDoc::AnyMethod.new(nil, 'm3')
    meth = cm2.add_method RDoc::AnyMethod.new(nil, 'm4')

    cm1.merge cm2

    expected = [
      RDoc::AnyMethod.new(nil, 'm1'),
      RDoc::AnyMethod.new(nil, 'm2'),
      RDoc::AnyMethod.new(nil, 'm3'),
      RDoc::AnyMethod.new(nil, 'm4'),
    ]

    expected.each do |a| a.parent = cm1 end

    assert_equal expected, cm1.method_list.sort
  end

  def test_parse
    tl1 = RDoc::TopLevel.new 'one.rb'
    tl2 = RDoc::TopLevel.new 'two.rb'

    cm = RDoc::ClassModule.new 'Klass'
    cm.add_comment 'comment 1', tl1
    cm.add_comment 'comment 2', tl2

    doc1 = @RM::Document.new @RM::Paragraph.new 'comment 1'
    doc1.file = tl1.absolute_name
    doc2 = @RM::Document.new @RM::Paragraph.new 'comment 2'
    doc2.file = tl2.absolute_name

    expected = @RM::Document.new doc1, doc2

    assert_equal expected, cm.parse(cm.comment_location)
  end

  def test_parse_comment_location
    tl1 = RDoc::TopLevel.new 'one.rb'
    tl2 = RDoc::TopLevel.new 'two.rb'

    cm = RDoc::ClassModule.new 'Klass'
    cm.add_comment 'comment 1', tl1
    cm.add_comment 'comment 2', tl2

    cm = Marshal.load Marshal.dump cm

    doc1 = @RM::Document.new @RM::Paragraph.new 'comment 1'
    doc1.file = tl1.absolute_name
    doc2 = @RM::Document.new @RM::Paragraph.new 'comment 2'
    doc2.file = tl2.absolute_name

    assert_same cm.comment_location, cm.parse(cm.comment_location)
  end

  def test_remove_nodoc_children
    parent = RDoc::ClassModule.new 'A'
    parent.modules_hash.replace 'B' => true, 'C' => true
    RDoc::TopLevel.all_modules_hash.replace 'A::B' => true

    parent.classes_hash.replace 'D' => true, 'E' => true
    RDoc::TopLevel.all_classes_hash.replace 'A::D' => true

    parent.remove_nodoc_children

    assert_equal %w[B], parent.modules_hash.keys
    assert_equal %w[D], parent.classes_hash.keys
  end

  def test_superclass
    assert_equal @c3_h1, @c3_h2.superclass
  end

  def test_update_aliases_class
    n1 = @xref_data.add_module RDoc::NormalClass, 'N1'
    n1_k2 = n1.add_module RDoc::NormalClass, 'N2'

    n1.add_module_alias n1_k2, 'A1', @xref_data

    n1_a1_c = n1.constants.find { |c| c.name == 'A1' }
    refute_nil n1_a1_c
    assert_equal n1_k2, n1_a1_c.is_alias_for, 'sanity check'

    n1.update_aliases

    n1_a1_k = @xref_data.find_class_or_module 'N1::A1'
    refute_nil n1_a1_k
    assert_equal n1_k2, n1_a1_k.is_alias_for
    refute_equal n1_k2, n1_a1_k

    assert_equal 1, n1_k2.aliases.length
    assert_equal n1_a1_k, n1_k2.aliases.first

    assert_equal 'N1::N2', n1_k2.full_name
    assert_equal 'N1::A1', n1_a1_k.full_name
  end

  def test_update_aliases_module
    n1 = @xref_data.add_module RDoc::NormalModule, 'N1'
    n1_n2 = n1.add_module RDoc::NormalModule, 'N2'

    n1.add_module_alias n1_n2, 'A1', @xref_data

    n1_a1_c = n1.constants.find { |c| c.name == 'A1' }
    refute_nil n1_a1_c
    assert_equal n1_n2, n1_a1_c.is_alias_for, 'sanity check'

    n1.update_aliases

    n1_a1_m = @xref_data.find_class_or_module 'N1::A1'
    refute_nil n1_a1_m
    assert_equal n1_n2, n1_a1_m.is_alias_for
    refute_equal n1_n2, n1_a1_m

    assert_equal 1, n1_n2.aliases.length
    assert_equal n1_a1_m, n1_n2.aliases.first

    assert_equal 'N1::N2', n1_n2.full_name
    assert_equal 'N1::A1', n1_a1_m.full_name
  end

  def test_update_aliases_reparent
    l1 = @xref_data.add_module RDoc::NormalModule, 'L1'
    l1_l2 = l1.add_module RDoc::NormalModule, 'L2'
    o1 = @xref_data.add_module RDoc::NormalModule, 'O1'

    o1.add_module_alias l1_l2, 'A1', @xref_data

    o1_a1_c = o1.constants.find { |c| c.name == 'A1' }
    refute_nil o1_a1_c
    assert_equal l1_l2, o1_a1_c.is_alias_for
    refute_equal l1_l2, o1_a1_c

    o1.update_aliases

    o1_a1_m = @xref_data.find_class_or_module 'O1::A1'
    refute_nil o1_a1_m
    assert_equal l1_l2, o1_a1_m.is_alias_for

    assert_equal 1, l1_l2.aliases.length
    assert_equal o1_a1_m, l1_l2.aliases[0]

    assert_equal 'L1::L2', l1_l2.full_name
    assert_equal 'O1::A1', o1_a1_m.full_name
  end

  def test_update_includes
    a = RDoc::Include.new 'M1', nil
    b = RDoc::Include.new 'M2', nil
    c = RDoc::Include.new 'C', nil

    @c1.add_include a
    @c1.add_include b
    @c1.add_include c
    @c1.ancestors # cache included modules

    @m1_m2.document_self = nil
    assert @m1_m2.remove_from_documentation?

    assert RDoc::TopLevel.all_modules_hash.key? @m1_m2.full_name
    refute RDoc::TopLevel.all_modules_hash[@m1_m2.full_name].nil?
    RDoc::TopLevel.remove_nodoc RDoc::TopLevel.all_modules_hash
    refute RDoc::TopLevel.all_modules_hash.key? @m1_m2.full_name

    @c1.update_includes

    assert_equal [a, c], @c1.includes
  end

  def test_update_includes_with_colons
    a = RDoc::Include.new 'M1', nil
    b = RDoc::Include.new 'M1::M2', nil
    c = RDoc::Include.new 'C', nil

    @c1.add_include a
    @c1.add_include b
    @c1.add_include c
    @c1.ancestors # cache included modules

    @m1_m2.document_self = nil
    assert @m1_m2.remove_from_documentation?

    assert RDoc::TopLevel.all_modules_hash.key? @m1_m2.full_name
    refute RDoc::TopLevel.all_modules_hash[@m1_m2.full_name].nil?
    RDoc::TopLevel.remove_nodoc RDoc::TopLevel.all_modules_hash
    refute RDoc::TopLevel.all_modules_hash.key? @m1_m2.full_name

    @c1.update_includes

    assert_equal [a, c], @c1.includes
  end

end

