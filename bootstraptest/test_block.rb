assert_equal %q{1}, %q{
  1.times{
    begin
      a = 1
    ensure
      foo = nil
    end
  }
}
assert_equal %q{2}, %q{
  [1,2,3].find{|x| x == 2}
}
assert_equal %q{2}, %q{
  class E
    include Enumerable
    def each(&block)
      [1, 2, 3].each(&block)
    end
  end
  E.new.find {|x| x == 2 }
}
assert_equal %q{6}, %q{
  sum = 0
  for x in [1, 2, 3]
    sum += x
  end
  sum
}
assert_equal %q{15}, %q{
  sum = 0
  for x in (1..5)
    sum += x
  end
  sum
}
assert_equal %q{0}, %q{
  sum = 0
  for x in []
    sum += x
  end
  sum
}
assert_equal %q{1}, %q{
  ans = []
  1.times{
    for n in 1..3
      a = n
      ans << a
    end
  }
}
assert_equal %q{1..3}, %q{
  ans = []
  for m in 1..3
    for n in 1..3
      a = [m, n]
      ans << a
    end
  end
}
assert_equal %q{[1, 2, 3]}, %q{
  (1..3).to_a
}
assert_equal %q{[4, 8, 12]}, %q{
  (1..3).map{|e|
    e * 4
  }
}
assert_equal %q{[1, 2, 3]}, %q{
  class C
    include Enumerable
    def each
      [1,2,3].each{|e|
        yield e
      }
    end
  end

  C.new.to_a
}
assert_equal %q{[4, 5, 6]}, %q{
  class C
    include Enumerable
    def each
      [1,2,3].each{|e|
        yield e
      }
    end
  end

  C.new.map{|e|
    e + 3
  }
}
assert_equal %q{100}, %q{
  def m
    yield
  end
  def n
    yield
  end

  m{
    n{
      100
    }
  }
}
assert_equal %q{20}, %q{
  def m
    yield 1
  end

  m{|ib|
    m{|jb|
      i = 20
    }
  }
}
assert_equal %q{2}, %q{
  def m
    yield 1
  end

  m{|ib|
    m{|jb|
      ib = 20
      kb = 2
    }
  }
}
assert_equal %q{3}, %q{
  def iter1
    iter2{
      yield
    }
  end

  def iter2
    yield
  end

  iter1{
    jb = 2
    iter1{
      jb = 3
    }
    jb
  }
}
assert_equal %q{2}, %q{
  def iter1
    iter2{
      yield
    }
  end

  def iter2
    yield
  end

  iter1{
    jb = 2
    iter1{
      jb
    }
    jb
  }
}
assert_equal %q{2}, %q{
  def m
    yield 1
  end
  m{|ib|
    ib*2
  }
}
assert_equal %q{92580}, %q{
  def m
    yield 12345, 67890
  end
  m{|ib,jb|
    ib*2+jb
  }
}
assert_equal %q{[10, nil]}, %q{
  def iter
    yield 10
  end

  a = nil
  [iter{|a|
    a
  }, a]
}
assert_equal %q{21}, %q{
  def iter
    yield 10
  end

  iter{|a|
    iter{|a|
      a + 1
    } + a
  }
}
assert_equal %q{[10, 20, 30, 40, nil, nil, nil, nil]}, %q{
  def iter
    yield 10, 20, 30, 40
  end

  a = b = c = d = nil
  iter{|a, b, c, d|
    [a, b, c, d]
  } + [a, b, c, d]
}
assert_equal %q{[10, 20, 30, 40, nil, nil]}, %q{
  def iter
    yield 10, 20, 30, 40
  end

  a = b = nil
  iter{|a, b, c, d|
    [a, b, c, d]
  } + [a, b]
}
assert_equal %q{[1]}, %q{
  $a = []

  def iter
    yield 1
  end

  def m
    x = iter{|x|
      $a << x
      y = 0
    }
  end
  m
  $a
}
assert_equal %q{[1, [2]]}, %q{
  def iter
    yield 1, 2
  end

  iter{|a, *b|
    [a, b]
  }
}
assert_equal %q{[[1, 2]]}, %q{
  def iter
    yield 1, 2
  end

  iter{|*a|
    [a]
  }
}
assert_equal %q{[1, 2, []]}, %q{
  def iter
    yield 1, 2
  end

  iter{|a, b, *c|
    [a, b, c]
  }
}
assert_equal %q{[1, 2, nil, []]}, %q{
  def iter
    yield 1, 2
  end

  iter{|a, b, c, *d|
    [a, b, c, d]
  }
}
assert_equal %q{1}, %q{
  def m
    yield
  end
  m{
    1
  }
}
assert_equal %q{15129}, %q{
  def m
    yield 123
  end
  m{|ib|
    m{|jb|
      ib*jb
    }
  }
}
assert_equal %q{2}, %q{
  def m a
    yield a
  end
  m(1){|ib|
    m(2){|jb|
      ib*jb
    }
  }
}
assert_equal %q{9}, %q{
  sum = 0
  3.times{|ib|
    2.times{|jb|
      sum += ib + jb
    }}
  sum
}
assert_equal %q{10}, %q{
  3.times{|bl|
    break 10
  }
}
assert_equal %q{[1, 2]}, %q{
  def iter
    yield 1,2,3
  end

  iter{|i, j|
    [i, j]
  }
}
assert_equal %q{[1, nil]}, %q{
  def iter
    yield 1
  end

  iter{|i, j|
    [i, j]
  }
}

assert_equal '0', %q{
def m()
end
m {|(v0,*,(*)),|}
m {|(*v0,(*)),|}
m {|(v0,*v1,(*)),|}
m {|((v0,*v1,v2)),|}
m {|(v0,*v1,v2),|}
m {|(v0,*v1,(v2)),|}
m {|((*),*v0,v1),|}
m {|((v0),*v1,v2),|}
m {|(v0,v1,*v2,v3),|}
m {|v0,(v1,*v2,v3),|}
m {|(v0,*v1,v2),v3,|}
m {|(v0,*v1,v2)|}
m {|(v0,*v1,v2),&v3|}
m {|(v0,*v1,v2),*|}
m {|(v0,*v1,v2),*,&v3|}
m {|*,(v0,*v1,v2)|}
m {|*,(v0,*v1,v2),&v3|}
m {|v0,*,(v1,*v2,v3)|}
m {|v0,*,(v1,*v2,v3),&v4|}
m {|(v0,*v1,v2),*,v3|}
m {|(v0,*v1,v2),*,v3,&v4|}
m {|(v0, *v1, v2)|}
m {|(*,v)|}
0
}, "block parameter (shouldn't SEGV: [ruby-dev:31143])"

assert_equal 'nil', %q{
  def m
    yield
  end
  m{|&b| b}.inspect
}, '[ruby-dev:31147]'

assert_equal 'nil', %q{
  def m()
    yield
  end
  m {|(v,(*))|}.inspect
}, '[ruby-dev:31160]'

assert_equal 'nil', %q{
  def m()
    yield
  end
  m {|(*,a,b)|}.inspect
}, '[ruby-dev:31153]'

assert_equal 'nil', %q{
  def m()
    yield
  end
  m {|((*))|}.inspect
}

assert_equal %q{[1, 1, [1, nil], [1, nil], [1, nil], [1, nil], [1, 1], 1, [1, nil], [1, nil], [1, nil], [1, nil], [[1, 1], [1, 1]], [1, 1], [1, 1], [1, 1], [1, nil], [1, nil], [[[1, 1], [1, 1]], [[1, 1], [1, 1]]], [[1, 1], [1, 1]], [[1, 1], [1, 1]], [[1, 1], [1, 1]], [1, 1], [1, 1], [[[[1, 1], [1, 1]], [[1, 1], [1, 1]]], [[[1, 1], [1, 1]], [[1, 1], [1, 1]]]], [[[1, 1], [1, 1]], [[1, 1], [1, 1]]], [[[1, 1], [1, 1]], [[1, 1], [1, 1]]], [[[1, 1], [1, 1]], [[1, 1], [1, 1]]], [[1, 1], [1, 1]], [[1, 1], [1, 1]]]}, %q{
def m(ary = [])
  yield(ary)
end

$ans = []
o = 1
5.times{
  v,(*) = o; $ans << o
  m(o){|(v,(*))| $ans << v}
  ((x, y)) = o; $ans << [x, y]
  m(o){|((x, y))| $ans << [x, y]}
  (((x, y))) = o; $ans << [x, y]
  m(o){|(((x, y)))| $ans << [x, y]}
  o = [o, o]
}; $ans
}

assert_equal '0', %q{
  def m()
    yield [0]
  end
  m {|*,v| v}.inspect
}, '[ruby-dev:31437]'
assert_equal '[0]', %q{
  def m
    yield [0]
  end
  m{|v, &b| v}.inspect
}, '[ruby-dev:31440]'
assert_equal 'ok', %q{
  begin
    lambda{|a|}.call(1, 2)
  rescue ArgumentError
    :ok
  else
    :ng
  end
}, '[ruby-dev:31464]'
assert_equal 'ok', %q{
  begin
    lambda{|&b|}.call(3)
  rescue ArgumentError
    :ok
  else
    :ng
  end
}, '[ruby-dev:31472]'
assert_equal 'ok', %q{
  class C
    def each
      yield [1,2]
      yield 1,2
    end
  end
  vs1 = []
  C.new.each {|*v| vs1 << v }
  vs2 = []
  C.new.to_enum.each {|*v| vs2 << v }
  vs1 == vs2 ? :ok : :ng
}, '[ruby-dev:32329]'

assert_normal_exit %q{
  e = [1,2,3].each
  10000.times {
    e = [e].each
  }
  Thread.new { GC.start }.join
}, '[ruby-dev:32604]'


assert_equal '[nil, []]', %q{
  def m() yield nil,[] end
  l = lambda {|*v| v}
  GC.stress=true
  r = m(&l)
  GC.stress=false
  r.inspect
}, '[ruby-dev:32567]'

assert_equal NilClass.to_s, %q{
  r = false; 1.times{|&b| r = b}; r.class
}

assert_equal 'ok', %q{
  class C
    define_method(:foo) do |arg, &block|
      if block then block.call else arg end
    end
  end
  C.new.foo("ng") {"ok"}
}, '[ruby-talk:266422]'

assert_equal 'ok', %q{
  class C
    define_method(:xyz) do |o, k, &block|
      block.call(o, k)
    end
  end
  C.new.xyz("o","k") {|o, k| o+k}
}, '[ruby-core:20544]'

assert_equal 'ok', %q{
  class C
    define_method(:xyz) do |*args, &block|
      block.call(*args)
    end
  end
  C.new.xyz("o","k") {|*args| args.join("")}
}, '[ruby-core:20544]'

assert_equal 'ok', %q{
  STDERR.reopen(STDOUT)
  class C
    define_method(:foo) do |&block|
      block.call if block
    end
    result = "ng"
    new.foo() {result = "ok"}
    result
  end
}

assert_equal "ok", %q{
  class Bar
    def bar; :ok; end
  end
  def foo
    yield(Bar.new) if block_given?
  end
  foo(&:bar)
}, '[ruby-core:14279]'

assert_normal_exit %q{
  class Controller
    def respond_to(&block)
      responder = Responder.new
      block.call(responder)
      responder.respond
    end
    def test_for_bug
      respond_to{|format|
        format.js{
          puts "in test"
          render{|obj|
            puts obj
          }
        }
      }
    end
    def render(&block)
      puts "in render"
    end
  end

  class Responder
    def method_missing(symbol, &block)
      puts "enter method_missing"
      @response = Proc.new{
        puts 'in method missing'
        block.call
      }
      puts "leave method_missing"
    end
    def respond
      @response.call
    end
  end
  t = Controller.new
  t.test_for_bug
}, '[ruby-core:14395]'

