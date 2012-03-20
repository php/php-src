assert_equal %q{[1, 2, 3]}, %q{
  def getproc &b
    b
  end

  def m
    yield
  end

  m{
    i = 1
    m{
      j = 2
      m{
        k = 3
        getproc{
          [i, j, k]
        }
      }
    }
  }.call
}
assert_equal %q{7}, %q{
  def make_proc(&b)
    b
  end

  def make_closure
    a = 0
    make_proc{
      a+=1
    }
  end

  cl = make_closure
  cl.call + cl.call * cl.call
}
assert_equal %q{ok}, %q{
  class C
    def foo
      :ok
    end
  end

  def block
    C.method(:new).to_proc
  end
  b = block()
  b.call.foo
}
assert_equal %q{[0, 1, :last, 0, 2, :last]}, %q{
  def proc &b
    b
  end

  pr = []
  proc{|i_b|
    p3 = proc{|j_b|
      pr << proc{|k_b|
        [i_b, j_b, k_b]
      }
    }
    p3.call(1)
    p3.call(2)
  }.call(0)

  pr[0].call(:last).concat pr[1].call(:last)
}
assert_equal %q{12}, %q{
  def iter
    yield
  end

  def getproc &b
    b
  end

  iter{
    bvar = 3
    getproc{
      bvar2 = 4
      bvar * bvar2
    }
  }.call
}
assert_equal %q{200}, %q{
  def iter
    yield
  end

  def getproc &b
    b
  end

  loc1 = 0
  pr1 = iter{
    bl1 = 1
    getproc{
      loc1 += 1
      bl1  += 1
      loc1 + bl1
    }
  }

  pr2 = iter{
    bl1 = 1
    getproc{
      loc1 += 1
      bl1  += 1
      loc1 + bl1
    }
  }

  pr1.call; pr2.call
  pr1.call; pr2.call
  pr1.call; pr2.call
  (pr1.call + pr2.call) * loc1
}
assert_equal %q{[1, 2]}, %q{
  def proc(&pr)
    pr
  end

  def m
    a = 1
    m2{
      a
    }
  end

  def m2
    b = 2
    proc{
      [yield, b]
    }
  end

  pr = m
  x = ['a', 1,2,3,4,5,6,7,8,9,0,
            1,2,3,4,5,6,7,8,9,0,
            1,2,3,4,5,6,7,8,9,0,
            1,2,3,4,5,6,7,8,9,0,
            1,2,3,4,5,6,7,8,9,0,]
  pr.call
}
assert_equal %q{1}, %q{
  def proc(&pr)
    pr
  end

  def m
    a = 1
    m2{
      a
    }
  end

  def m2
    b = 2
    proc{
      [yield, b]
    }
    100000.times{|x|
      "#{x}"
    }
    yield
  end
  m
}
assert_equal %q{[:C, :C]}, %q{
  Const = :top
  class C
    Const = :C
    $pr = proc{
      (1..2).map{
        Const
      }
    }
  end
  $pr.call
}
assert_equal %q{top}, %q{
  Const = :top
  class C
    Const = :C
  end
  pr = proc{
    Const
  }
  C.class_eval %q{
    pr.call
  }
}
assert_equal %q{1}, %q{
  def m(&b)
    b
  end

  m{|e_proctest| e_proctest}.call(1)
}
assert_equal %q{12}, %q{
  def m(&b)
    b
  end

  m{|e_proctest1, e_proctest2|
    a = e_proctest1 * e_proctest2 * 2
    a * 3
  }.call(1, 2)
}
assert_equal %q{[[], [1], [1, 2], [1, 2, 3]]}, %q{
  [
  Proc.new{|*args| args}.call(),
  Proc.new{|*args| args}.call(1),
  Proc.new{|*args| args}.call(1, 2),
  Proc.new{|*args| args}.call(1, 2, 3),
  ]
}
assert_equal %q{[[nil, []], [1, []], [1, [2]], [1, [2, 3]]]}, %q{
  [
  Proc.new{|a, *b| [a, b]}.call(),
  Proc.new{|a, *b| [a, b]}.call(1),
  Proc.new{|a, *b| [a, b]}.call(1, 2),
  Proc.new{|a, *b| [a, b]}.call(1, 2, 3),
  ]
}
assert_equal %q{0}, %q{
  pr = proc{
    $SAFE
  }
  $SAFE = 1
  pr.call
}
assert_equal %q{[1, 0]}, %q{
  pr = proc{
    $SAFE += 1
  }
  [pr.call, $SAFE]
}
assert_equal %q{1}, %q{
  def m(&b)
    b
  end
  m{1}.call
}
assert_equal %q{3}, %q{
  def m(&b)
    b
  end

  m{
    a = 1
    a + 2
  }.call
}
assert_equal %Q{ok\n}, %q{
  class A; def get_block; proc {puts "ok"} end end
  block = A.new.get_block
  GC.start
  block.call
}, '[ruby-core:14885]'

assert_equal 'ok', %q{
  a = lambda {|x, y, &b| b }
  b = a.curry[1]
  if b.call(2){} == nil
    :ng
  else
    :ok
  end
}, '[ruby-core:15551]'

assert_equal 'ok', %q{
  lambda {
    break :ok
    :ng
  }.call
}, '[ruby-dev:34646]'

assert_equal %q{[:bar, :foo]}, %q{
  def foo
    klass = Class.new do
      define_method(:bar) do
        return :bar
      end
    end
    [klass.new.bar, :foo]
  end
  foo
}, "[ ruby-Bugs-19304 ]"

assert_equal 'ok', %q{
   $x = :ok
   def def7(x, y)
      x[y]
      $x = :ng
   end
   def test_def7
      def7(lambda {|x| x.call}, Proc.new {return})
      $x = :ng
   end
   test_def7
   $x
}, '[ruby-core:17164]'

assert_equal 'ok', %q{
  lambda { a = lambda { return }; $x = :ng; a[]; $x = :ok }.call
  $x
}, '[ruby-core:17164]'

assert_equal 'ok', %q{
  lambda { a = lambda { break }; $x = :ng; a[]; $x = :ok }.call
  $x
}, '[ruby-core:17164]'

assert_equal 'ok', %q{
  def def8
    $x = :ng
    lambda { a = Proc.new { return }; a[]}.call
    $x = :ok
  end
  def8
  $x
}, '[ruby-core:17164]'


assert_equal 'ok', %q{
   def def9
      lambda {|a| $x = :ok; a[]; $x = :ng }.call(Proc.new { return })
      $x = :ng
   end
   def9
   $x
}, '[ruby-core:17164]'

assert_equal 'ok', %q{
   def def10
     $x = :ng
     lambda { 1.times { return } }.call
     $x = :ok
   end
   $x = :ok
   def10
   $x
}, '[ruby-core:17164]'

assert_equal 'ok', %q{
   def def11
      yield
   end
   begin
      lambda { def11 { return } }.call
   rescue LocalJumpError
      :ng
   else
      :ok
   end
}, '[ruby-core:17164]'

assert_equal 'ok', %q{
   def def12
      b = Proc.new { $x = :ng; lambda { return }.call; $x = :ok }.call
   end
   def12
   $x
}, '[ruby-core:17164]'

assert_equal 'ok', %q{
  def m
    pr = proc{
      proc{
        return :ok
      }
    }.call
    pr.call
    :ng
  end
  m()
}

assert_equal 'ok', %q{
  class Foo
    def call_it
      p = Proc.new
      p.call
    end
  end

  def give_it
    proc { :ok }
  end

  f = Foo.new
  a_proc = give_it
  f.call_it(&give_it())
}, '[ruby-core:15711]'

assert_equal 'foo!', %q{
  class FooProc < Proc
    def initialize
      @foo = "foo!"
    end

    def bar
      @foo
    end
  end

  def bar
    FooProc.new &lambda{
      p 1
    }
  end

  fp = bar(&lambda{
    p 2
  })

  fp.bar
}, 'Subclass of Proc'

assert_equal 'ok', %q{
  o = Object.new
  def o.write(s); end
  $stderr = o
  at_exit{
    print $!.message
  }
  raise "ok"
}

assert_equal 'ok', %q{
  lambda do
    class A
      class B
        proc{return :ng}.call
      end
    end
  end.call
  :ok
}

assert_equal 'ok', %q{
  $proc = proc{return}
  begin
    lambda do
      class A
        class B
          $proc.call
        end
      end
    end.call
    :ng
  rescue LocalJumpError
    :ok
  end
}

