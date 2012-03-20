require 'test/unit'
require 'stringio'

class TestRubyYield < Test::Unit::TestCase

  def test_ary_each
    ary = [1]
    ary.each {|a, b, c, d| assert_equal [1,nil,nil,nil], [a,b,c,d] }
    ary.each {|a, b, c| assert_equal [1,nil,nil], [a,b,c] }
    ary.each {|a, b| assert_equal [1,nil], [a,b] }
    ary.each {|a| assert_equal 1, a }
  end

  def test_hash_each
    h = {:a => 1}
    h.each do |k, v|
      assert_equal :a, k
      assert_equal 1, v
    end
    h.each do |kv|
      assert_equal [:a, 1], kv
    end
  end

  def test_yield_0
    assert_equal 1, iter0 { 1 }
    assert_equal 2, iter0 { 2 }
  end

  def iter0
    yield
  end

  def test_yield_1
    iter1([]) {|a, b| assert_equal [nil,nil], [a, b] }
    iter1([1]) {|a, b| assert_equal [1,nil], [a, b] }
    iter1([1, 2]) {|a, b| assert_equal [1,2], [a,b] }
    iter1([1, 2, 3]) {|a, b| assert_equal [1,2], [a,b] }

    iter1([]) {|a| assert_equal [], a }
    iter1([1]) {|a| assert_equal [1], a }
    iter1([1, 2]) {|a| assert_equal [1,2], a }
    iter1([1, 2, 3]) {|a| assert_equal [1,2,3], a }
  end

  def iter1(args)
    yield args
  end

  def test_yield2
    def iter2_1() yield 1, *[2, 3] end
    iter2_1 {|a, b, c| assert_equal [1,2,3], [a,b,c] }
    def iter2_2() yield 1, *[] end
    iter2_2 {|a, b, c| assert_equal [1,nil,nil], [a,b,c] }
    def iter2_3() yield 1, *[2] end
    iter2_3 {|a, b, c| assert_equal [1,2,nil], [a,b,c] }
  end

  def test_yield_nested
    [[1, [2, 3]]].each {|a, (b, c)|
      assert_equal [1,2,3], [a,b,c]
    }
    [[1, [2, 3]]].map {|a, (b, c)|
      assert_equal [1,2,3], [a,b,c]
    }
  end

  def test_with_enum
    obj = Object
    def obj.each
      yield(*[])
    end
    obj.each{|*v| assert_equal([], [], '[ruby-dev:32392]')}
    obj.to_enum.each{|*v| assert_equal([], [], '[ruby-dev:32392]')}
  end

  def block_args_unleashed
    yield(1,2,3,4,5)
  end

  def test_block_args_unleashed
    r = block_args_unleashed {|a,b=1,*c,d,e|
      [a,b,c,d,e]
    }
    assert_equal([1,2,[3],4,5], r, "[ruby-core:19485]")
  end
end

require_relative 'sentence'
class TestRubyYieldGen < Test::Unit::TestCase
  Syntax = {
    :exp => [["0"],
             ["nil"],
             ["false"],
             ["[]"],
             ["[",:exps,"]"]],
    :exps => [[:exp],
              [:exp,",",:exps]],
    :opt_block_param => [[],
                         [:block_param_def]],
    :block_param_def => [['|', '|'],
                         ['|', :block_param, '|']],
    :block_param => [[:f_arg, ",", :f_rest_arg, :opt_f_block_arg],
                     [:f_arg, ","],
                     [:f_arg, ',', :f_rest_arg, ",", :f_arg, :opt_f_block_arg],
                     [:f_arg, :opt_f_block_arg],
                     [:f_rest_arg, :opt_f_block_arg],
                     [:f_rest_arg, ',', :f_arg, :opt_f_block_arg],
                     [:f_block_arg]],
    :f_arg => [[:f_arg_item],
               [:f_arg, ',', :f_arg_item]],
    :f_rest_arg => [['*', "var"],
                    ['*']],
    :opt_f_block_arg => [[',', :f_block_arg],
                         []],
    :f_block_arg => [['&', 'var']],
    :f_arg_item => [[:f_norm_arg],
                    ['(', :f_margs, ')']],
    :f_margs => [[:f_marg_list],
                 [:f_marg_list, ',', '*', :f_norm_arg],
                 [:f_marg_list, ',', '*', :f_norm_arg, ',', :f_marg_list],
                 [:f_marg_list, ',', '*'],
                 [:f_marg_list, ',', '*',              ',', :f_marg_list],
                 [                   '*', :f_norm_arg],
                 [                   '*', :f_norm_arg, ',', :f_marg_list],
                 [                   '*'],
                 [                   '*',              ',', :f_marg_list]],
    :f_marg_list => [[:f_marg],
                     [:f_marg_list, ',', :f_marg]],
    :f_marg => [[:f_norm_arg],
                ['(', :f_margs, ')']],
    :f_norm_arg => [['var']],

    :command_args => [[:open_args]],
    :open_args => [[' ',:call_args],
                   ['(', ')'],
                   ['(', :call_args2, ')']],
    :call_args =>  [[:command],
                    [           :args,               :opt_block_arg],
                    [                       :assocs, :opt_block_arg],
                    [           :args, ',', :assocs, :opt_block_arg],
                    [                                    :block_arg]],
    :call_args2 => [[:arg, ',', :args,               :opt_block_arg],
                    [:arg, ',',                          :block_arg],
                    [                       :assocs, :opt_block_arg],
                    [:arg, ',',             :assocs, :opt_block_arg],
                    [:arg, ',', :args, ',', :assocs, :opt_block_arg],
                    [                                    :block_arg]],

    :command_args_noblock => [[:open_args_noblock]],
    :open_args_noblock => [[' ',:call_args_noblock],
                   ['(', ')'],
                   ['(', :call_args2_noblock, ')']],
    :call_args_noblock =>  [[:command],
                    [           :args],
                    [                       :assocs],
                    [           :args, ',', :assocs]],
    :call_args2_noblock => [[:arg, ',', :args],
                            [                       :assocs],
                            [:arg, ',',             :assocs],
                            [:arg, ',', :args, ',', :assocs]],

    :command => [],
    :args => [[:arg],
              ["*",:arg],
              [:args,",",:arg],
              [:args,",","*",:arg]],
    :arg => [[:exp]],
    :assocs => [[:assoc],
                [:assocs, ',', :assoc]],
    :assoc => [[:arg, '=>', :arg],
               ['label', ':', :arg]],
    :opt_block_arg => [[',', :block_arg],
                       []],
    :block_arg => [['&', :arg]],
    #:test => [['def m() yield', :command_args_noblock, ' end; r = m {', :block_param_def, 'vars', '}; undef m; r']]
    :test_proc => [['def m() yield', :command_args_noblock, ' end; r = m {', :block_param_def, 'vars', '}; undef m; r']],
    :test_lambda => [['def m() yield', :command_args_noblock, ' end; r = m(&lambda {', :block_param_def, 'vars', '}); undef m; r']],
    :test_enum => [['o = Object.new; def o.each() yield', :command_args_noblock, ' end; r1 = r2 = nil; o.each {|*x| r1 = x }; o.to_enum.each {|*x| r2 = x }; [r1, r2]']]
  }

  def rename_var(obj)
    vars = []
    r = obj.subst('var') {
      var = "v#{vars.length}"
      vars << var
      var
    }
    return r, vars
  end

  def split_by_comma(ary)
    return [] if ary.empty?
    result = [[]]
    ary.each {|e|
      if e == ','
        result << []
      else
        result.last << e
      end
    }
    result
  end

  def emu_return_args(*vs)
    vs
  end

  def emu_eval_args(args)
    if args.last == []
      args = args[0...-1]
    end
    code = "emu_return_args(#{args.map {|a| a.join('') }.join(",")})"
    eval code, nil, 'generated_code_in_emu_eval_args'
  end

  def emu_bind_single(arg, param, result_binding)
    #p [:emu_bind_single, arg, param]
    if param.length == 1 && String === param[0] && /\A[a-z0-9]+\z/ =~ param[0]
      result_binding[param[0]] = arg
    elsif param.length == 1 && Array === param[0] && param[0][0] == '(' && param[0][-1] == ')'
      arg = [arg] unless Array === arg
      emu_bind_params(arg, split_by_comma(param[0][1...-1]), false, result_binding)
    else
      raise "unexpected param: #{param.inspect}"
    end
    result_binding
  end

  def emu_bind_params(args, params, islambda, result_binding={})
    #p [:emu_bind_params, args, params]
    if params.last == [] # extra comma
      params.pop
    end

    star_index = nil
    params.each_with_index {|par, i|
      star_index = i if par[0] == '*'
    }

    if islambda
      if star_index
        if args.length < params.length - 1
          throw :emuerror, ArgumentError
        end
      else
        if args.length != params.length
          throw :emuerror, ArgumentError
        end
      end
    end

    # TRICK #2 : adjust mismatch on number of arguments
    if star_index
      pre_params = params[0...star_index]
      rest_param = params[star_index]
      post_params = params[(star_index+1)..-1]
      pre_params.each {|par| emu_bind_single(args.shift, par, result_binding) }
      if post_params.length <= args.length
        post_params.reverse_each {|par| emu_bind_single(args.pop, par, result_binding) }
      else
        post_params.each {|par| emu_bind_single(args.shift, par, result_binding) }
      end
      if rest_param != ['*']
        emu_bind_single(args, rest_param[1..-1], result_binding)
      end
    else
      params.each_with_index {|par, i|
        emu_bind_single(args[i], par, result_binding)
      }
    end

    #p [args, params, result_binding]

    result_binding
  end

  def emu_bind(t, islambda)
    #puts
    #p t
    command_args_noblock = t[1]
    block_param_def = t[3]
    command_args_noblock = command_args_noblock.expand {|a| !(a[0] == '[' && a[-1] == ']') }
    block_param_def = block_param_def.expand {|a| !(a[0] == '(' && a[-1] == ')') }

    if command_args_noblock.to_a[0] == ' '
      args = command_args_noblock.to_a[1..-1]
    elsif command_args_noblock.to_a[0] == '(' && command_args_noblock.to_a[-1] == ')'
      args = command_args_noblock.to_a[1...-1]
    else
      raise "unexpected command_args_noblock: #{command_args_noblock.inspect}"
    end
    args = emu_eval_args(split_by_comma(args))

    params = block_param_def.to_a[1...-1]
    params = split_by_comma(params)

    #p [:emu0, args, params]

    result_binding = {}

    if params.last && params.last[0] == '&'
      result_binding[params.last[1]] = nil
      params.pop
    end

    if !islambda
      # TRICK #1 : single array argument is expanded if there are two or more params.
      # * block parameter is not counted.
      # * extra comma after single param forces the expansion.
      if args.length == 1 && Array === args[0] && 1 < params.length
        args = args[0]
      end
    end

    emu_bind_params(args, params, islambda, result_binding)
    #p result_binding
    result_binding
  end

  def emu(t, vars, islambda)
    catch(:emuerror) {
      emu_binding = emu_bind(t, islambda)
      vars.map {|var| emu_binding.fetch(var, "NOVAL") }
    }
  end

  def disable_stderr
    begin
      save_stderr = $stderr
      $stderr = StringIO.new
      yield
    ensure
      $stderr = save_stderr
    end
  end

  def check_nofork(t, islambda=false)
    t, vars = rename_var(t)
    t = t.subst('vars') { " [#{vars.join(",")}]" }
    emu_values = emu(t, vars, islambda)
    s = t.to_s
    #print "#{s}\t\t"
    #STDOUT.flush
    eval_values = disable_stderr {
      begin
        eval(s, nil, 'generated_code_in_check_nofork')
      rescue ArgumentError
        ArgumentError
      end
    }
    #success = emu_values == eval_values ? 'succ' : 'fail'
    #puts "eval:#{vs_ev.inspect[1...-1].delete(' ')}\temu:#{vs_emu.inspect[1...-1].delete(' ')}\t#{success}"
    assert_equal(emu_values, eval_values, s)
  end

  def test_yield
    syntax = Sentence.expand_syntax(Syntax)
    Sentence.each(syntax, :test_proc, 4) {|t|
      check_nofork(t)
    }
  end

  def test_yield_lambda
    syntax = Sentence.expand_syntax(Syntax)
    Sentence.each(syntax, :test_lambda, 4) {|t|
      check_nofork(t, true)
    }
  end

  def test_yield_enum
    syntax = Sentence.expand_syntax(Syntax)
    Sentence.each(syntax, :test_enum, 4) {|t|
      code = t.to_s
      r1, r2 = disable_stderr {
        eval(code, nil, 'generated_code_in_test_yield_enum')
      }
      assert_equal(r1, r2, "#{t}")
    }
  end

end
