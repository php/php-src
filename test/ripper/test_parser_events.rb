begin
  require_relative 'dummyparser'
  require_relative '../ruby/envutil'
  require 'test/unit'
  ripper_test = true
  module TestRipper; end
rescue LoadError
end

class TestRipper::ParserEvents < Test::Unit::TestCase

  # should be enabled
  def test_event_coverage
    dispatched = Ripper::PARSER_EVENTS.map {|event,*| event }
    dispatched.each do |e|
      assert_equal true, respond_to?("test_#{e}", true),
                   "event not tested: #{e.inspect}"
    end
  end

  def parse(str, nm = nil, &bl)
    dp = DummyParser.new(str)
    dp.hook(*nm, &bl) if nm
    dp.parse.to_s
  end

  def test_program
    thru_program = false
    assert_equal '[void()]', parse('', :on_program) {thru_program = true}
    assert_equal true, thru_program
  end

  def test_stmts_new
    assert_equal '[void()]', parse('')
  end

  def test_stmts_add
    assert_equal '[ref(nil)]', parse('nil')
    assert_equal '[ref(nil),ref(nil)]', parse('nil;nil')
    assert_equal '[ref(nil),ref(nil),ref(nil)]', parse('nil;nil;nil')
  end

  def test_void_stmt
    assert_equal '[void()]', parse('')
    assert_equal '[void()]', parse('; ;')
  end

  def test_var_ref
    assert_equal '[assign(var_field(a),ref(a))]', parse('a=a')
    assert_equal '[ref(nil)]', parse('nil')
    assert_equal '[ref(true)]', parse('true')
  end

  def test_vcall
    assert_equal '[vcall(a)]', parse('a')
  end

  def test_BEGIN
    assert_equal '[BEGIN([void()])]', parse('BEGIN{}')
    assert_equal '[BEGIN([ref(nil)])]', parse('BEGIN{nil}')
  end

  def test_END
    assert_equal '[END([void()])]', parse('END{}')
    assert_equal '[END([ref(nil)])]', parse('END{nil}')
  end

  def test_alias
    assert_equal '[alias(symbol_literal(a),symbol_literal(b))]', parse('alias a b')
  end

  def test_var_alias
    assert_equal '[valias($a,$g)]', parse('alias $a $g')
  end

  def test_alias_error
    assert_equal '[aliaserr(valias($a,$1))]', parse('alias $a $1')
  end

  def test_arglist
    assert_equal '[fcall(m,[])]', parse('m()')
    assert_equal '[fcall(m,[1])]', parse('m(1)')
    assert_equal '[fcall(m,[1,2])]', parse('m(1,2)')
    assert_equal '[fcall(m,[*vcall(r)])]', parse('m(*r)')
    assert_equal '[fcall(m,[1,*vcall(r)])]', parse('m(1,*r)')
    assert_equal '[fcall(m,[1,2,*vcall(r)])]', parse('m(1,2,*r)')
    assert_equal '[fcall(m,[&vcall(r)])]', parse('m(&r)')
    assert_equal '[fcall(m,[1,&vcall(r)])]', parse('m(1,&r)')
    assert_equal '[fcall(m,[1,2,&vcall(r)])]', parse('m(1,2,&r)')
    assert_equal '[fcall(m,[*vcall(a),&vcall(b)])]', parse('m(*a,&b)')
    assert_equal '[fcall(m,[1,*vcall(a),&vcall(b)])]', parse('m(1,*a,&b)')
    assert_equal '[fcall(m,[1,2,*vcall(a),&vcall(b)])]', parse('m(1,2,*a,&b)')
  end

  def test_args_add
    thru_args_add = false
    parse('m(a)', :on_args_add) {thru_args_add = true}
    assert_equal true, thru_args_add
  end

  def test_args_add_block
    thru_args_add_block = false
    parse('m(&b)', :on_args_add_block) {thru_args_add_block = true}
    assert_equal true, thru_args_add_block
  end

  def test_args_add_star
    thru_args_add_star = false
    parse('m(*a)', :on_args_add_star) {thru_args_add_star = true}
    assert_equal true, thru_args_add_star
    thru_args_add_star = false
    parse('m(*a, &b)', :on_args_add_star) {thru_args_add_star = true}
    assert_equal true, thru_args_add_star
  end

  def test_args_new
    thru_args_new = false
    parse('m()', :on_args_new) {thru_args_new = true}
    assert_equal true, thru_args_new
  end

  def test_arg_paren
    # FIXME
  end

  def test_aref
    assert_equal '[aref(vcall(v),[1])]', parse('v[1]')
    assert_equal '[aref(vcall(v),[1,2])]', parse('v[1,2]')
  end

  def test_assoclist_from_args
    thru_assoclist_from_args = false
    parse('{a=>b}', :on_assoclist_from_args) {thru_assoclist_from_args = true}
    assert_equal true, thru_assoclist_from_args
  end

  def test_assocs
    assert_equal '[fcall(m,[assocs(assoc(1,2))])]', parse('m(1=>2)')
    assert_equal '[fcall(m,[assocs(assoc(1,2),assoc(3,4))])]', parse('m(1=>2,3=>4)')
    assert_equal '[fcall(m,[3,assocs(assoc(1,2))])]', parse('m(3,1=>2)')
  end

  def test_assoc_new
    thru_assoc_new = false
    parse('{a=>b}', :on_assoc_new) {thru_assoc_new = true}
    assert_equal true, thru_assoc_new
  end

  def test_aref_field
    assert_equal '[assign(aref_field(vcall(a),[1]),2)]', parse('a[1]=2')
  end

  def test_arg_ambiguous
    thru_arg_ambiguous = false
    parse('m //', :on_arg_ambiguous) {thru_arg_ambiguous = true}
    assert_equal true, thru_arg_ambiguous
  end

  def test_operator_ambiguous
    thru_operator_ambiguous = false
    parse('a=1; a %[]', :on_operator_ambiguous) {thru_operator_ambiguous = true}
    assert_equal true, thru_operator_ambiguous
  end

  def test_array   # array literal
    assert_equal '[array([1,2,3])]', parse('[1,2,3]')
    assert_equal '[array([abc,def])]', parse('%w[abc def]')
    assert_equal '[array([abc,def])]', parse('%W[abc def]')
  end

  def test_assign   # generic assignment
    assert_equal '[assign(var_field(v),1)]', parse('v=1')
  end

  def test_assign_error
    thru_assign_error = false
    parse('$` = 1', :on_assign_error) {thru_assign_error = true}
    assert_equal true, thru_assign_error
    thru_assign_error = false
    parse('$`, _ = 1', :on_assign_error) {thru_assign_error = true}
    assert_equal true, thru_assign_error

    thru_assign_error = false
    parse('self::X = 1', :on_assign_error) {thru_assign_error = true}
    assert_equal false, thru_assign_error
    parse('def m\n self::X = 1\nend', :on_assign_error) {thru_assign_error = true}
    assert_equal true, thru_assign_error

    thru_assign_error = false
    parse('X = 1', :on_assign_error) {thru_assign_error = true}
    assert_equal false, thru_assign_error
    parse('def m\n X = 1\nend', :on_assign_error) {thru_assign_error = true}
    assert_equal true, thru_assign_error

    thru_assign_error = false
    parse('::X = 1', :on_assign_error) {thru_assign_error = true}
    assert_equal false, thru_assign_error
    parse('def m\n ::X = 1\nend', :on_assign_error) {thru_assign_error = true}
    assert_equal true, thru_assign_error
  end

  def test_bare_assoc_hash
    thru_bare_assoc_hash = false
    parse('x[a=>b]', :on_bare_assoc_hash) {thru_bare_assoc_hash = true}
    assert_equal true, thru_bare_assoc_hash
    thru_bare_assoc_hash = false
    parse('x[1, a=>b]', :on_bare_assoc_hash) {thru_bare_assoc_hash = true}
    assert_equal true, thru_bare_assoc_hash
    thru_bare_assoc_hash = false
    parse('x(a=>b)', :on_bare_assoc_hash) {thru_bare_assoc_hash = true}
    assert_equal true, thru_bare_assoc_hash
    thru_bare_assoc_hash = false
    parse('x(1, a=>b)', :on_bare_assoc_hash) {thru_bare_assoc_hash = true}
    assert_equal true, thru_bare_assoc_hash
  end

  def test_begin
    thru_begin = false
    parse('begin end', :on_begin) {thru_begin = true}
    assert_equal true, thru_begin
  end

  def test_binary
    thru_binary = nil
    %w"and or + - * / % ** | ^ & <=> > >= < <= == === != =~ !~ << >> && ||".each do |op|
      thru_binary = false
      parse("a #{op} b", :on_binary) {thru_binary = true}
      assert_equal true, thru_binary
    end
  end

  def test_blockarg
    thru_blockarg = false
    parse("def a(&b) end", :on_blockarg) {thru_blockarg = true}
    assert_equal true, thru_blockarg
    thru_blockarg = false
    parse("def a(x, &b) end", :on_blockarg) {thru_blockarg = true}
    assert_equal true, thru_blockarg

    thru_blockarg = false
    parse("proc{|&b|}", :on_blockarg) {thru_blockarg = true}
    assert_equal true, thru_blockarg
    thru_blockarg = false
    parse("proc{|x, &b|}", :on_blockarg) {thru_blockarg = true}
    assert_equal true, thru_blockarg
    thru_blockarg = false
    parse("proc{|&b;y|}", :on_blockarg) {thru_blockarg = true}
    assert_equal true, thru_blockarg
    thru_blockarg = false
    parse("proc{|&b,x;y|}", :on_blockarg) {thru_blockarg = true}
    assert_equal true, thru_blockarg

    thru_blockarg = false
    parse("proc do |&b| end", :on_blockarg) {thru_blockarg = true}
    assert_equal true, thru_blockarg
    thru_blockarg = false
    parse("proc do |&b, x| end", :on_blockarg) {thru_blockarg = true}
    assert_equal true, thru_blockarg
    thru_blockarg = false
    parse("proc do |&b;y| end", :on_blockarg) {thru_blockarg = true}
    assert_equal true, thru_blockarg
    thru_blockarg = false
    parse("proc do |&b, x;y| end", :on_blockarg) {thru_blockarg = true}
    assert_equal true, thru_blockarg
  end

  def test_block_var
    thru_block_var = false
    parse("proc{||}", :on_block_var) {thru_block_var = true}
    assert_equal true, thru_block_var
    thru_block_var = false
    parse("proc{| |}", :on_block_var) {thru_block_var = true}
    assert_equal true, thru_block_var
    thru_block_var = false
    parse("proc{|x|}", :on_block_var) {thru_block_var = true}
    assert_equal true, thru_block_var
    thru_block_var = false
    parse("proc{|;y|}", :on_block_var) {thru_block_var = true}
    assert_equal true, thru_block_var
    thru_block_var = false
    parse("proc{|x;y|}", :on_block_var) {thru_block_var = true}
    assert_equal true, thru_block_var

    thru_block_var = false
    parse("proc do || end", :on_block_var) {thru_block_var = true}
    assert_equal true, thru_block_var
    thru_block_var = false
    parse("proc do | | end", :on_block_var) {thru_block_var = true}
    assert_equal true, thru_block_var
    thru_block_var = false
    parse("proc do |x| end", :on_block_var) {thru_block_var = true}
    assert_equal true, thru_block_var
    thru_block_var = false
    parse("proc do |;y| end", :on_block_var) {thru_block_var = true}
    assert_equal true, thru_block_var
    thru_block_var = false
    parse("proc do |x;y| end", :on_block_var) {thru_block_var = true}
    assert_equal true, thru_block_var
  end

  def test_block_var_add_block
    # not used
  end

  def test_block_var_add_star
    # not used
  end

  def test_bodystmt
    thru_bodystmt = false
    parse("class X\nend", :on_bodystmt) {thru_bodystmt = true}
    assert_equal true, thru_bodystmt
  end

  def test_call
    bug2233 = '[ruby-core:26165]'
    tree = nil

    thru_call = false
    assert_nothing_raised {
      tree = parse("self.foo", :on_call) {thru_call = true}
    }
    assert_equal true, thru_call
    assert_equal "[call(ref(self),.,foo)]", tree
    thru_call = false
    assert_nothing_raised(bug2233) {
      tree = parse("foo.()", :on_call) {thru_call = true}
    }
    assert_equal true, thru_call
    assert_equal "[call(vcall(foo),.,call,[])]", tree
  end

  def test_excessed_comma
    thru_excessed_comma = false
    parse("proc{|x,|}", :on_excessed_comma) {thru_excessed_comma = true}
    assert_equal true, thru_excessed_comma
    thru_excessed_comma = false
    parse("proc{|x,y,|}", :on_excessed_comma) {thru_excessed_comma = true}
    assert_equal true, thru_excessed_comma

    thru_excessed_comma = false
    parse("proc do |x,| end", :on_excessed_comma) {thru_excessed_comma = true}
    assert_equal true, thru_excessed_comma
    thru_excessed_comma = false
    parse("proc do |x,y,| end", :on_excessed_comma) {thru_excessed_comma = true}
    assert_equal true, thru_excessed_comma
  end

  def test_heredoc
    bug1921 = '[ruby-core:24855]'
    thru_heredoc_beg = false
    tree = parse("<<EOS\nheredoc\nEOS\n", :on_heredoc_beg) {thru_heredoc_beg = true}
    assert_equal true, thru_heredoc_beg
    assert_match(/string_content\(\),heredoc\n/, tree, bug1921)
    heredoc = nil
    parse("<<EOS\nheredoc1\nheredoc2\nEOS\n", :on_string_add) {|e, n, s| heredoc = s}
    assert_equal("heredoc1\nheredoc2\n", heredoc, bug1921)
    heredoc = nil
    parse("<<-EOS\nheredoc1\nheredoc2\n\tEOS\n", :on_string_add) {|e, n, s| heredoc = s}
    assert_equal("heredoc1\nheredoc2\n", heredoc, bug1921)
  end

  def test_massign
    thru_massign = false
    parse("a, b = 1, 2", :on_massign) {thru_massign = true}
    assert_equal true, thru_massign
  end

  def test_mlhs_add
    thru_mlhs_add = false
    parse("a, b = 1, 2", :on_mlhs_add) {thru_mlhs_add = true}
    assert_equal true, thru_mlhs_add
  end

  def test_mlhs_add_star
    bug2232 = '[ruby-core:26163]'
    bug4364 = '[ruby-core:35078]'

    thru_mlhs_add_star = false
    tree = parse("a, *b = 1, 2", :on_mlhs_add_star) {thru_mlhs_add_star = true}
    assert_equal true, thru_mlhs_add_star
    assert_match(/mlhs_add_star\(mlhs_add\(mlhs_new\(\),a\),b\)/, tree)
    thru_mlhs_add_star = false
    tree = parse("a, *b, c = 1, 2", :on_mlhs_add_star) {thru_mlhs_add_star = true}
    assert_equal true, thru_mlhs_add_star
    assert_match(/mlhs_add\(mlhs_add_star\(mlhs_add\(mlhs_new\(\),a\),b\),mlhs_add\(mlhs_new\(\),c\)\)/, tree, bug2232)
    thru_mlhs_add_star = false
    tree = parse("a, *, c = 1, 2", :on_mlhs_add_star) {thru_mlhs_add_star = true}
    assert_equal true, thru_mlhs_add_star
    assert_match(/mlhs_add\(mlhs_add_star\(mlhs_add\(mlhs_new\(\),a\)\),mlhs_add\(mlhs_new\(\),c\)\)/, tree, bug4364)
    thru_mlhs_add_star = false
    tree = parse("*b, c = 1, 2", :on_mlhs_add_star) {thru_mlhs_add_star = true}
    assert_equal true, thru_mlhs_add_star
    assert_match(/mlhs_add\(mlhs_add_star\(mlhs_new\(\),b\),mlhs_add\(mlhs_new\(\),c\)\)/, tree, bug4364)
    thru_mlhs_add_star = false
    tree = parse("*, c = 1, 2", :on_mlhs_add_star) {thru_mlhs_add_star = true}
    assert_equal true, thru_mlhs_add_star
    assert_match(/mlhs_add\(mlhs_add_star\(mlhs_new\(\)\),mlhs_add\(mlhs_new\(\),c\)\)/, tree, bug4364)
  end

  def test_mlhs_new
    thru_mlhs_new = false
    parse("a, b = 1, 2", :on_mlhs_new) {thru_mlhs_new = true}
    assert_equal true, thru_mlhs_new
  end

  def test_mlhs_paren
    thru_mlhs_paren = false
    parse("a, b = 1, 2", :on_mlhs_paren) {thru_mlhs_paren = true}
    assert_equal false, thru_mlhs_paren
    thru_mlhs_paren = false
    parse("(a, b) = 1, 2", :on_mlhs_paren) {thru_mlhs_paren = true}
    assert_equal true, thru_mlhs_paren
  end

  def test_brace_block
    thru_brace_block = false
    parse('proc {}', :on_brace_block) {thru_brace_block = true}
    assert_equal true, thru_brace_block
  end

  def test_break
    thru_break = false
    parse('proc {break}', :on_break) {thru_break = true}
    assert_equal true, thru_break
  end

  def test_case
    thru_case = false
    parse('case foo when true; end', :on_case) {thru_case = true}
    assert_equal true, thru_case
  end

  def test_class
    thru_class = false
    parse('class Foo; end', :on_class) {thru_class = true}
    assert_equal true, thru_class
  end

  def test_class_name_error
    thru_class_name_error = false
    parse('class foo; end', :on_class_name_error) {thru_class_name_error = true}
    assert_equal true, thru_class_name_error
  end

  def test_command
    thru_command = false
    parse('foo a b', :on_command) {thru_command = true}
    assert_equal true, thru_command
  end

  def test_command_call
    thru_command_call = false
    parse('foo.bar a, b', :on_command_call) {thru_command_call = true}
    assert_equal true, thru_command_call
  end

  def test_const_ref
    thru_const_ref = false
    parse('class A;end', :on_const_ref) {thru_const_ref = true}
    assert_equal true, thru_const_ref
    thru_const_ref = false
    parse('module A;end', :on_const_ref) {thru_const_ref = true}
    assert_equal true, thru_const_ref
  end

  def test_const_path_field
    thru_const_path_field = false
    parse('foo::X = 1', :on_const_path_field) {thru_const_path_field = true}
    assert_equal true, thru_const_path_field
  end

  def test_const_path_ref
    thru_const_path_ref = false
    parse('foo::X', :on_const_path_ref) {thru_const_path_ref = true}
    assert_equal true, thru_const_path_ref
  end

  def test_def
    thru_def = false
    parse('def foo; end', :on_def) {
      thru_def = true
    }
    assert_equal true, thru_def
    assert_equal '[def(foo,[],bodystmt([void()]))]', parse('def foo ;end')
  end

  def test_defined
    thru_defined = false
    parse('defined?(x)', :on_defined) {thru_defined = true}
    assert_equal true, thru_defined
  end

  def test_defs
    thru_defs = false
    parse('def foo.bar; end', :on_defs) {thru_defs = true}
    assert_equal true, thru_defs
  end

  def test_do_block
    thru_do_block = false
    parse('proc do end', :on_do_block) {thru_do_block = true}
    assert_equal true, thru_do_block
  end

  def test_dot2
    thru_dot2 = false
    parse('a..b', :on_dot2) {thru_dot2 = true}
    assert_equal true, thru_dot2
  end

  def test_dot3
    thru_dot3 = false
    parse('a...b', :on_dot3) {thru_dot3 = true}
    assert_equal true, thru_dot3
  end

  def test_dyna_symbol
    thru_dyna_symbol = false
    parse(':"#{foo}"', :on_dyna_symbol) {thru_dyna_symbol = true}
    assert_equal true, thru_dyna_symbol
  end

  def test_else
    thru_else = false
    parse('if foo; bar else zot end', :on_else) {thru_else = true}
    assert_equal true, thru_else
  end

  def test_elsif
    thru_elsif = false
    parse('if foo; bar elsif qux; zot end', :on_elsif) {thru_elsif = true}
    assert_equal true, thru_elsif
  end

  def test_ensure
    thru_ensure = false
    parse('begin foo ensure bar end', :on_ensure) {thru_ensure = true}
    assert_equal true, thru_ensure
  end

  def test_fcall
    thru_fcall = false
    parse('foo()', :on_fcall) {thru_fcall = true}
    assert_equal true, thru_fcall
  end

  def test_field
    thru_field = false
    parse('foo.x = 1', :on_field) {thru_field = true}
    assert_equal true, thru_field
  end

  def test_for
    thru_for = false
    parse('for i in foo; end', :on_for) {thru_for = true}
    assert_equal true, thru_for
  end

  def test_hash
    thru_hash = false
    parse('{1=>2}', :on_hash) {thru_hash = true}
    assert_equal true, thru_hash
    thru_hash = false
    parse('{a: 2}', :on_hash) {thru_hash = true}
    assert_equal true, thru_hash
  end

  def test_if
    thru_if = false
    parse('if false; end', :on_if) {thru_if = true}
    assert_equal true, thru_if
  end

  def test_if_mod
    thru_if_mod = false
    parse('nil if nil', :on_if_mod) {thru_if_mod = true}
    assert_equal true, thru_if_mod
  end

  def test_ifop
    thru_ifop = false
    parse('a ? b : c', :on_ifop) {thru_ifop = true}
    assert_equal true, thru_ifop
  end

  def test_lambda
    thru_lambda = false
    parse('->{}', :on_lambda) {thru_lambda = true}
    assert_equal true, thru_lambda
  end

  def test_magic_comment
    thru_magic_comment = false
    parse('# -*- bug-5753: ruby-dev:44984 -*-', :on_magic_comment) {|*x|thru_magic_comment = x}
    assert_equal [:on_magic_comment, "bug_5753", "ruby-dev:44984"], thru_magic_comment
  end

  def test_method_add_block
    thru_method_add_block = false
    parse('a {}', :on_method_add_block) {thru_method_add_block = true}
    assert_equal true, thru_method_add_block
    thru_method_add_block = false
    parse('a do end', :on_method_add_block) {thru_method_add_block = true}
    assert_equal true, thru_method_add_block
  end

  def test_method_add_arg
    thru_method_add_arg = false
    parse('a()', :on_method_add_arg) {thru_method_add_arg = true}
    assert_equal true, thru_method_add_arg
    thru_method_add_arg = false
    parse('a {}', :on_method_add_arg) {thru_method_add_arg = true}
    assert_equal true, thru_method_add_arg
    thru_method_add_arg = false
    parse('a.b(1)', :on_method_add_arg) {thru_method_add_arg = true}
    assert_equal true, thru_method_add_arg
    thru_method_add_arg = false
    parse('a::b(1)', :on_method_add_arg) {thru_method_add_arg = true}
    assert_equal true, thru_method_add_arg
  end

  def test_module
    thru_module = false
    parse('module A; end', :on_module) {thru_module = true}
    assert_equal true, thru_module
  end

  def test_mrhs_add
    thru_mrhs_add = false
    parse('a = a, b', :on_mrhs_add) {thru_mrhs_add = true}
    assert_equal true, thru_mrhs_add
  end

  def test_mrhs_add_star
    thru_mrhs_add_star = false
    parse('a = a, *b', :on_mrhs_add_star) {thru_mrhs_add_star = true}
    assert_equal true, thru_mrhs_add_star
  end

  def test_mrhs_new
    thru_mrhs_new = false
    parse('a = *a', :on_mrhs_new) {thru_mrhs_new = true}
    assert_equal true, thru_mrhs_new
  end

  def test_mrhs_new_from_args
    thru_mrhs_new_from_args = false
    parse('a = a, b', :on_mrhs_new_from_args) {thru_mrhs_new_from_args = true}
    assert_equal true, thru_mrhs_new_from_args
  end

  def test_next
    thru_next = false
    parse('a {next}', :on_next) {thru_next = true}
    assert_equal true, thru_next
  end

  def test_opassign
    thru_opassign = false
    parse('a += b', :on_opassign) {thru_opassign = true}
    assert_equal true, thru_opassign
    thru_opassign = false
    parse('a -= b', :on_opassign) {thru_opassign = true}
    assert_equal true, thru_opassign
    thru_opassign = false
    parse('a *= b', :on_opassign) {thru_opassign = true}
    assert_equal true, thru_opassign
    thru_opassign = false
    parse('a /= b', :on_opassign) {thru_opassign = true}
    assert_equal true, thru_opassign
    thru_opassign = false
    parse('a %= b', :on_opassign) {thru_opassign = true}
    assert_equal true, thru_opassign
    thru_opassign = false
    parse('a **= b', :on_opassign) {thru_opassign = true}
    assert_equal true, thru_opassign
    thru_opassign = false
    parse('a &= b', :on_opassign) {thru_opassign = true}
    assert_equal true, thru_opassign
    thru_opassign = false
    parse('a |= b', :on_opassign) {thru_opassign = true}
    assert_equal true, thru_opassign
    thru_opassign = false
    parse('a <<= b', :on_opassign) {thru_opassign = true}
    assert_equal true, thru_opassign
    thru_opassign = false
    parse('a >>= b', :on_opassign) {thru_opassign = true}
    assert_equal true, thru_opassign
    thru_opassign = false
    parse('a &&= b', :on_opassign) {thru_opassign = true}
    assert_equal true, thru_opassign
    thru_opassign = false
    parse('a ||= b', :on_opassign) {thru_opassign = true}
    assert_equal true, thru_opassign
  end

  def test_opassign_error
    thru_opassign = []
    events = [:on_opassign, :on_assign_error]
    parse('a::X ||= c 1', events) {|a,*b|
      thru_opassign << a
    }
    assert_equal events, thru_opassign
  end

  def test_param_error
    thru_param_error = false
    parse('def foo(A) end', :on_param_error) {thru_param_error = true}
    assert_equal true, thru_param_error
    thru_param_error = false
    parse('def foo($a) end', :on_param_error) {thru_param_error = true}
    assert_equal true, thru_param_error
    thru_param_error = false
    parse('def foo(@a) end', :on_param_error) {thru_param_error = true}
    assert_equal true, thru_param_error
    thru_param_error = false
    parse('def foo(@@a) end', :on_param_error) {thru_param_error = true}
    assert_equal true, thru_param_error
  end

  def test_params
    thru_params = false
    parse('a {||}', :on_params) {thru_params = true}
    assert_equal true, thru_params
    thru_params = false
    parse('a {|x|}', :on_params) {thru_params = true}
    assert_equal true, thru_params
    thru_params = false
    parse('a {|*x|}', :on_params) {thru_params = true}
    assert_equal true, thru_params
  end

  def test_paren
    thru_paren = false
    parse('()', :on_paren) {thru_paren = true}
    assert_equal true, thru_paren
  end

  def test_parse_error
    thru_parse_error = false
    parse('<>', :on_parse_error) {thru_parse_error = true}
    assert_equal true, thru_parse_error
  end

  def test_qwords_add
    thru_qwords_add = false
    parse('%w[a]', :on_qwords_add) {thru_qwords_add = true}
    assert_equal true, thru_qwords_add
  end

  def test_qwords_new
    thru_qwords_new = false
    parse('%w[]', :on_qwords_new) {thru_qwords_new = true}
    assert_equal true, thru_qwords_new
  end

  def test_redo
    thru_redo = false
    parse('redo', :on_redo) {thru_redo = true}
    assert_equal true, thru_redo
  end

  def test_regexp_add
    thru_regexp_add = false
    parse('/foo/', :on_regexp_add) {thru_regexp_add = true}
    assert_equal true, thru_regexp_add
  end

  def test_regexp_literal
    thru_regexp_literal = false
    parse('//', :on_regexp_literal) {thru_regexp_literal = true}
    assert_equal true, thru_regexp_literal
  end

  def test_regexp_new
    thru_regexp_new = false
    parse('//', :on_regexp_new) {thru_regexp_new = true}
    assert_equal true, thru_regexp_new
  end

  def test_rescue
    thru_rescue = false
    parsed = parse('begin; 1; rescue => e; 2; end', :on_rescue) {thru_rescue = true}
    assert_equal true, thru_rescue
    assert_match /1.*rescue/, parsed
    assert_match /rescue\(,var_field\(e\),\[2\]\)/, parsed
  end

  def test_rescue_mod
    thru_rescue_mod = false
    parsed = parse('1 rescue 2', :on_rescue_mod) {thru_rescue_mod = true}
    assert_equal true, thru_rescue_mod
    bug4716 = '[ruby-core:36248]'
    assert_equal "[rescue_mod(1,2)]", parsed, bug4716
  end

  def test_rest_param
    thru_rest_param = false
    parse('def a(*) end', :on_rest_param) {thru_rest_param = true}
    assert_equal true, thru_rest_param
    thru_rest_param = false
    parse('def a(*x) end', :on_rest_param) {thru_rest_param = true}
    assert_equal true, thru_rest_param
  end

  def test_retry
    thru_retry = false
    parse('retry', :on_retry) {thru_retry = true}
    assert_equal true, thru_retry
  end

  def test_return
    thru_return = false
    parse('return a', :on_return) {thru_return = true}
    assert_equal true, thru_return
  end

  def test_return0
    thru_return0 = false
    parse('return', :on_return0) {thru_return0 = true}
    assert_equal true, thru_return0
  end

  def test_sclass
    thru_sclass = false
    parse('class << a; end', :on_sclass) {thru_sclass = true}
    assert_equal true, thru_sclass
  end

  def test_string_add
    thru_string_add = false
    parse('"aa"', :on_string_add) {thru_string_add = true}
    assert_equal true, thru_string_add
  end

  def test_string_concat
    thru_string_concat = false
    parse('"a" "b"', :on_string_concat) {thru_string_concat = true}
    assert_equal true, thru_string_concat
  end

  def test_string_content
    thru_string_content = false
    parse('""', :on_string_content) {thru_string_content = true}
    assert_equal true, thru_string_content
    thru_string_content = false
    parse('"a"', :on_string_content) {thru_string_content = true}
    assert_equal true, thru_string_content
    thru_string_content = false
    parse('%[a]', :on_string_content) {thru_string_content = true}
    assert_equal true, thru_string_content
    thru_string_content = false
    parse('\'a\'', :on_string_content) {thru_string_content = true}
    assert_equal true, thru_string_content
    thru_string_content = false
    parse('%<a>', :on_string_content) {thru_string_content = true}
    assert_equal true, thru_string_content
    thru_string_content = false
    parse('%!a!', :on_string_content) {thru_string_content = true}
    assert_equal true, thru_string_content
    thru_string_content = false
    parse('%q!a!', :on_string_content) {thru_string_content = true}
    assert_equal true, thru_string_content
    thru_string_content = false
    parse('%Q!a!', :on_string_content) {thru_string_content = true}
    assert_equal true, thru_string_content
  end

  def test_string_dvar
    thru_string_dvar = false
    parse('"#$a"', :on_string_dvar) {thru_string_dvar = true}
    assert_equal true, thru_string_dvar
    thru_string_dvar = false
    parse('\'#$a\'', :on_string_dvar) {thru_string_dvar = true}
    assert_equal false, thru_string_dvar
    thru_string_dvar = false
    parse('"#@a"', :on_string_dvar) {thru_string_dvar = true}
    assert_equal true, thru_string_dvar
    thru_string_dvar = false
    parse('\'#@a\'', :on_string_dvar) {thru_string_dvar = true}
    assert_equal false, thru_string_dvar
    thru_string_dvar = false
    parse('"#@@a"', :on_string_dvar) {thru_string_dvar = true}
    assert_equal true, thru_string_dvar
    thru_string_dvar = false
    parse('\'#@@a\'', :on_string_dvar) {thru_string_dvar = true}
    assert_equal false, thru_string_dvar
    thru_string_dvar = false
    parse('"#$1"', :on_string_dvar) {thru_string_dvar = true}
    assert_equal true, thru_string_dvar
    thru_string_dvar = false
    parse('\'#$1\'', :on_string_dvar) {thru_string_dvar = true}
    assert_equal false, thru_string_dvar
  end

  def test_string_embexpr
    thru_string_embexpr = false
    parse('"#{}"', :on_string_embexpr) {thru_string_embexpr = true}
    assert_equal true, thru_string_embexpr
    thru_string_embexpr = false
    parse('\'#{}\'', :on_string_embexpr) {thru_string_embexpr = true}
    assert_equal false, thru_string_embexpr
  end

  def test_string_literal
    thru_string_literal = false
    parse('""', :on_string_literal) {thru_string_literal = true}
    assert_equal true, thru_string_literal
  end

  def test_super
    thru_super = false
    parse('super()', :on_super) {thru_super = true}
    assert_equal true, thru_super
  end

  def test_symbol
    thru_symbol = false
    parse(':a', :on_symbol) {thru_symbol = true}
    assert_equal true, thru_symbol
    thru_symbol = false
    parse(':$a', :on_symbol) {thru_symbol = true}
    assert_equal true, thru_symbol
    thru_symbol = false
    parse(':@a', :on_symbol) {thru_symbol = true}
    assert_equal true, thru_symbol
    thru_symbol = false
    parse(':@@a', :on_symbol) {thru_symbol = true}
    assert_equal true, thru_symbol
    thru_symbol = false
    parse(':==', :on_symbol) {thru_symbol = true}
    assert_equal true, thru_symbol
  end

  def test_symbol_literal
    thru_symbol_literal = false
    parse(':a', :on_symbol_literal) {thru_symbol_literal = true}
    assert_equal true, thru_symbol_literal
  end

  def test_top_const_field
    thru_top_const_field = false
    parse('::A=1', :on_top_const_field) {thru_top_const_field = true}
    assert_equal true, thru_top_const_field
  end

  def test_top_const_ref
    thru_top_const_ref = false
    parse('::A', :on_top_const_ref) {thru_top_const_ref = true}
    assert_equal true, thru_top_const_ref
  end

  def test_unary
    thru_unary = false
    parse('not a 1, 2', :on_unary) {thru_unary = true}
    assert_equal true, thru_unary
    thru_unary = false
    parse('not (a)', :on_unary) {thru_unary = true}
    assert_equal true, thru_unary
    thru_unary = false
    parse('!a', :on_unary) {thru_unary = true}
    assert_equal true, thru_unary
    thru_unary = false
    parse('-10', :on_unary) {thru_unary = true}
    assert_equal true, thru_unary
    thru_unary = false
    parse('-10*2', :on_unary) {thru_unary = true}
    assert_equal true, thru_unary
    thru_unary = false
    parse('-10.1', :on_unary) {thru_unary = true}
    assert_equal true, thru_unary
    thru_unary = false
    parse('-10.1*2', :on_unary) {thru_unary = true}
    assert_equal true, thru_unary
    thru_unary = false
    parse('-a', :on_unary) {thru_unary = true}
    assert_equal true, thru_unary
    thru_unary = false
    parse('+a', :on_unary) {thru_unary = true}
    assert_equal true, thru_unary
    thru_unary = false
    parse('~a', :on_unary) {thru_unary = true}
    assert_equal true, thru_unary
    thru_unary = false
    parse('not()', :on_unary) {thru_unary = true}
    assert_equal true, thru_unary
  end

  def test_undef
    thru_undef = false
    parse('undef a', :on_undef) {thru_undef = true}
    assert_equal true, thru_undef
    thru_undef = false
    parse('undef <=>', :on_undef) {thru_undef = true}
    assert_equal true, thru_undef
    thru_undef = false
    parse('undef a, b', :on_undef) {thru_undef = true}
    assert_equal true, thru_undef
  end

  def test_unless
    thru_unless = false
    parse('unless a; end', :on_unless) {thru_unless = true}
    assert_equal true, thru_unless
  end

  def test_unless_mod
    thru_unless_mod = false
    parse('nil unless a', :on_unless_mod) {thru_unless_mod = true}
    assert_equal true, thru_unless_mod
  end

  def test_until
    thru_until = false
    parse('until a; end', :on_until) {thru_until = true}
    assert_equal true, thru_until
  end

  def test_until_mod
    thru_until_mod = false
    parse('nil until a', :on_until_mod) {thru_until_mod = true}
    assert_equal true, thru_until_mod
  end

  def test_var_field
    thru_var_field = false
    parse('a = 1', :on_var_field) {thru_var_field = true}
    assert_equal true, thru_var_field
    thru_var_field = false
    parse('a += 1', :on_var_field) {thru_var_field = true}
    assert_equal true, thru_var_field
  end

  def test_when
    thru_when = false
    parse('case a when b; end', :on_when) {thru_when = true}
    assert_equal true, thru_when
    thru_when = false
    parse('case when a; end', :on_when) {thru_when = true}
    assert_equal true, thru_when
  end

  def test_while
    thru_while = false
    parse('while a; end', :on_while) {thru_while = true}
    assert_equal true, thru_while
  end

  def test_while_mod
    thru_while_mod = false
    parse('nil while a', :on_while_mod) {thru_while_mod = true}
    assert_equal true, thru_while_mod
  end

  def test_word_add
    thru_word_add = false
    parse('%W[a]', :on_word_add) {thru_word_add = true}
    assert_equal true, thru_word_add
  end

  def test_word_new
    thru_word_new = false
    parse('%W[a]', :on_word_new) {thru_word_new = true}
    assert_equal true, thru_word_new
  end

  def test_words_add
    thru_words_add = false
    parse('%W[a]', :on_words_add) {thru_words_add = true}
    assert_equal true, thru_words_add
  end

  def test_words_new
    thru_words_new = false
    parse('%W[]', :on_words_new) {thru_words_new = true}
    assert_equal true, thru_words_new
  end

  def test_xstring_add
    thru_xstring_add = false
    parse('`x`', :on_xstring_add) {thru_xstring_add = true}
    assert_equal true, thru_xstring_add
  end

  def test_xstring_literal
    thru_xstring_literal = false
    parse('``', :on_xstring_literal) {thru_xstring_literal = true}
    assert_equal true, thru_xstring_literal
  end

  def test_xstring_new
    thru_xstring_new = false
    parse('``', :on_xstring_new) {thru_xstring_new = true}
    assert_equal true, thru_xstring_new
  end

  def test_yield
    thru_yield = false
    parse('yield a', :on_yield) {thru_yield = true}
    assert_equal true, thru_yield
  end

  def test_yield0
    thru_yield0 = false
    parse('yield', :on_yield0) {thru_yield0 = true}
    assert_equal true, thru_yield0
  end

  def test_zsuper
    thru_zsuper = false
    parse('super', :on_zsuper) {thru_zsuper = true}
    assert_equal true, thru_zsuper
  end

  def test_local_variables
    cmd = 'command(w,[regexp_literal(regexp_add(regexp_new(),25 # ),/)])'
    div = 'binary(ref(w),/,25)'
    var = '[w]'
    bug1939 = '[ruby-core:24923]'

    assert_equal("[#{cmd}]", parse('w /25 # /'), bug1939)
    assert_equal("[assign(var_field(w),1),#{div}]", parse("w = 1; w /25 # /"), bug1939)
    assert_equal("[fcall(p,[],&block([w],[#{div}]))]", parse("p{|w|w /25 # /\n}"), bug1939)
    assert_equal("[def(p,[w],bodystmt([#{div}]))]", parse("def p(w)\nw /25 # /\nend"), bug1939)
  end

  def test_block_variables
    assert_equal("[fcall(proc,[],&block([],[void()]))]", parse("proc{|;y|}"))
    if defined?(Process::RLIMIT_AS)
      assert_in_out_err(["-I#{File.dirname(__FILE__)}", "-rdummyparser"],
                        'Process.setrlimit(Process::RLIMIT_AS,100*1024*1024); puts DummyParser.new("proc{|;y|!y}").parse',
                        ["[fcall(proc,[],&block([],[unary(!,ref(y))]))]"], [], '[ruby-dev:39423]')
    end
  end

  def test_unterminated_regexp
    compile_error = false
    parse('/', :compile_error) {|e, msg| compile_error = msg}
    assert_equal("unterminated regexp meets end of file", compile_error)
  end
end if ripper_test
