assert_equal %q{ok}, %q{
  def m
    a = :ok
    $b = binding
  end
  m
  eval('a', $b)
}
assert_equal %q{[:ok, :ok2]}, %q{
  def m
    a = :ok
    $b = binding
  end
  m
  eval('b = :ok2', $b)
  eval('[a, b]', $b)
}
assert_equal %q{[nil, 1]}, %q{
  $ans = []
  def m
    $b = binding
  end
  m
  $ans << eval(%q{
    $ans << eval(%q{
      a
    }, $b)
    a = 1
  }, $b)
  $ans
}
assert_equal %q{C}, %q{
  Const = :top
  class C
    Const = :C
    def m
      binding
    end
  end
  eval('Const', C.new.m)
}
assert_equal %q{top}, %q{
  Const = :top
  a = 1
  class C
    Const = :C
    def m
      eval('Const', TOPLEVEL_BINDING)
    end
  end
  C.new.m
}
assert_equal %q{:ok
ok}, %q{
  class C
    $b = binding
  end
  eval %q{
    def m
      :ok
    end
  }, $b
  p C.new.m
}
assert_equal %q{ok}, %q{
  b = proc{
    a = :ok
    binding
  }.call
  a = :ng
  eval("a", b)
}
assert_equal %q{C}, %q{
  class C
    def foo
      binding
    end
  end
  C.new.foo.eval("self.class.to_s")
}
assert_equal %q{1}, %q{
  eval('1')
}
assert_equal %q{1}, %q{
  eval('a=1; a')
}
assert_equal %q{1}, %q{
  a = 1
  eval('a')
}
assert_equal %q{ok}, %q{
  __send__ :eval, %{
    :ok
  }
}
assert_equal %q{ok}, %q{
  1.__send__ :instance_eval, %{
    :ok
  }
}
assert_equal %q{1}, %q{
  1.instance_eval{
    self
  }
}
assert_equal %q{foo}, %q{
  'foo'.instance_eval{
    self
  }
}
assert_equal %q{1}, %q{
  class Fixnum
    Const = 1
  end
  1.instance_eval %{
    Const
  }
}
assert_equal %q{top}, %q{
  Const = :top
  class C
    Const = :C
  end
  C.module_eval{
    Const
  }
}
assert_equal %q{C}, %q{
  Const = :top
  class C
    Const = :C
  end
  C.class_eval %{
    def m
      Const
    end
  }
  C.new.m
}
assert_equal %q{top}, %q{
  Const = :top
  class C
    Const = :C
  end
  C.class_eval{
    def m
      Const
    end
  }
  C.new.m
}
assert_equal %q{[:top, :C, :top, :C]}, %q{
  Const = :top
  class C
    Const = :C
  end
  $nest = false
  $ans = []
  def m
    $ans << Const
    C.module_eval %{
      $ans << Const
      Boo = false unless defined? Boo
      unless $nest
        $nest = true
        m
      end
    }
  end
  m
  $ans
}
assert_equal %q{[10, main]}, %q{
  $nested = false
  $ans = []
  $pr = proc{
    $ans << self
    unless $nested
      $nested = true
      $pr.call
    end
  }
  class C
    def initialize &b
      10.instance_eval(&b)
    end
  end
  C.new(&$pr)
  $ans
}

%w[break next redo].each do |keyword|
  assert_match %r"Can't escape from eval with #{keyword}\z", %{
    begin
      eval "0 rescue #{keyword}"
    rescue SyntaxError => e
      e.message
    end
  }, '[ruby-dev:31372]'
end

assert_normal_exit %q{
  STDERR.reopen(STDOUT)
  class Foo
     def self.add_method
       class_eval("def some-bad-name; puts 'hello' unless @some_variable.some_function(''); end")
     end
  end
  Foo.add_method
}, '[ruby-core:14556] reported by Frederick Cheung'

assert_equal 'ok', %q{
  class Module
    def my_module_eval(&block)
      module_eval(&block)
    end
  end
  class String
    Integer.my_module_eval do
      def hoge; end
    end
  end
  if Integer.instance_methods(false).map{|m|m.to_sym}.include?(:hoge) &&
     !String.instance_methods(false).map{|m|m.to_sym}.include?(:hoge)
    :ok
  else
    :ng
  end
}, "[ruby-dev:34236]"

assert_equal 'ok', %q{
  begin
    eval("class nil::Foo; end")
    :ng
  rescue Exception
    :ok
  end
}

assert_equal 'ok', %q{
  begin
    0.instance_eval { def m() :m end }
    1.m
    :ng
  rescue Exception
    :ok
  end
}, '[ruby-dev:34579]'

assert_equal 'ok', %q{
  begin
    12.instance_eval { @@a }
  rescue NameError
    :ok
  end
}, '[ruby-core:16794]'

assert_equal 'ok', %q{
  begin
    12.instance_exec { @@a }
  rescue NameError
    :ok
  end
}, '[ruby-core:16794]'

assert_equal 'ok', %q{
  begin
    nil.instance_eval {
      def a() :a end
    }
  rescue TypeError
    :ok
  end
}, '[ruby-core:16796]'

assert_equal 'ok', %q{
  begin
    nil.instance_exec {
      def a() :a end
    }
  rescue TypeError
    :ok
  end
}, '[ruby-core:16796]'

assert_normal_exit %q{
  eval("", method(:proc).call {}.binding)
}

assert_equal "", %q{
  b = binding
  10.times{
    eval('', b)
  }
  begin
    eval('1.times{raise}', b)
  rescue => e
    e.message
  end
}, '[ruby-dev:35392]'

assert_equal "[:x]", %q{
  def kaboom!
    yield.eval("local_variables")
  end

  for x in enum_for(:kaboom!)
    binding
  end
}, '[ruby-core:25125]'

assert_normal_exit %q{
  hash = {}
  ("aaaa".."matz").each_with_index do |s, i|
    hash[s] = i
  end
  begin
    eval "class C; @@h = #{hash.inspect}; end"
  rescue SystemStackError
  end
}, '[ruby-core:25714]'
