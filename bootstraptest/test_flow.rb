assert_equal %q{[1, 2, 4, 5, 6, 7, 8]}, %q{$a = []; begin;  ; $a << 1
  [1,2].each{; $a << 2
    break; $a << 3
  }; $a << 4
  begin; $a << 5
  ensure; $a << 6
  end; $a << 7
; $a << 8
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 2, 3, 5, 6, 7, 8]}, %q{$a = []; begin;  ; $a << 1
  begin; $a << 2
    [1,2].each do; $a << 3
      break; $a << 4
    end; $a << 5
  ensure; $a << 6
  end; $a << 7
; $a << 8
; rescue Exception; $a << 99; end; $a}
assert_equal %q{ok}, %q{
  ["a"].inject("ng"){|x,y|
    break :ok
  }
}
assert_equal %q{ok}, %q{
  unless ''.respond_to? :lines
    class String
      def lines
        self
      end
    end
  end

  ('a').lines.map{|e|
    break :ok
  }
}
assert_equal %q{[1, 2, 4, 5]}, %q{$a = []; begin; ; $a << 1
  ["a"].inject("ng"){|x,y|; $a << 2
    break :ok; $a << 3
  }; $a << 4
; $a << 5
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 2, 4, 5]}, %q{$a = []; begin; ; $a << 1
  ('a'..'b').map{|e|; $a << 2
    break :ok; $a << 3
  }; $a << 4
; $a << 5
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 2, 3, 5, 7, 8]}, %q{$a = []; begin;  ; $a << 1
  [1,2].each do; $a << 2
    begin; $a << 3
      break; $a << 4
    ensure; $a << 5
    end; $a << 6
  end; $a << 7
; $a << 8
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 2, 3, 4, 5, 6, 9, 10]}, %q{$a = []; begin; ; $a << 1
  i=0; $a << 2
  while i<3; $a << 3
    i+=1; $a << 4
    begin; $a << 5
    ensure; $a << 6
      break; $a << 7
    end; $a << 8
  end; $a << 9
; $a << 10
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 2, 3, 4, 5, 7, 10, 11]}, %q{$a = []; begin; ; $a << 1
  i=0; $a << 2
  while i<3; $a << 3
    i+=1; $a << 4
    begin; $a << 5
      raise; $a << 6
    ensure; $a << 7
      break; $a << 8
    end; $a << 9
  end; $a << 10
; $a << 11
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 2, 3, 4, 5, 7, 10, 11]}, %q{$a = []; begin; ; $a << 1
  i=0; $a << 2
  while i<3; $a << 3
    i+=1; $a << 4
    begin; $a << 5
      raise; $a << 6
    rescue; $a << 7
      break; $a << 8
    end; $a << 9
  end; $a << 10
; $a << 11
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 2, 3, 5, 8, 9]}, %q{$a = []; begin;  ; $a << 1
  [1,2].each do; $a << 2
    begin; $a << 3
      raise StandardError; $a << 4
    ensure; $a << 5
      break; $a << 6
    end; $a << 7
  end; $a << 8
; $a << 9
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 2, 3, 5, 8, 9]}, %q{$a = []; begin;  ; $a << 1
  [1,2].each do; $a << 2
    begin; $a << 3
      raise StandardError; $a << 4
    rescue; $a << 5
      break; $a << 6
    end; $a << 7
  end; $a << 8
; $a << 9
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 2, 3, 4, 6, 8, 10, 11]}, %q{$a = []; begin;  ; $a << 1
  [1,2].each do; $a << 2
    begin; $a << 3
      begin; $a << 4
        break; $a << 5
      ensure; $a << 6
      end; $a << 7
    ensure; $a << 8
    end; $a << 9
  end; $a << 10
; $a << 11
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 2, 3, 4, 5, 6, 7, 8, 10, 13, 3, 4, 5, 6, 7, 8, 10, 13, 3, 4, 5, 6, 7, 8, 10, 13, 14, 15]}, %q{$a = []; begin; ; $a << 1
  i = 0; $a << 2
  while i<3; $a << 3
    i+=1; $a << 4
    j = 0; $a << 5
    while j<3; $a << 6
      j+=1; $a << 7
      begin; $a << 8
        raise; $a << 9
      rescue; $a << 10
        break; $a << 11
      end; $a << 12
    end; $a << 13
  end; $a << 14
; $a << 15
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 14, 6, 7, 8, 9, 11, 14, 6, 7, 8, 9, 11, 14, 15, 3, 4, 5, 6, 7, 8, 9, 11, 14, 6, 7, 8, 9, 11, 14, 6, 7, 8, 9, 11, 14, 15, 3, 4, 5, 6, 7, 8, 9, 11, 14, 6, 7, 8, 9, 11, 14, 6, 7, 8, 9, 11, 14, 15, 16, 17]}, %q{$a = []; begin; ; $a << 1
  i = 0; $a << 2
  while i<3; $a << 3
    i+=1; $a << 4
    j = 0; $a << 5
    while j<3; $a << 6
      j+=1; $a << 7
      1.times{; $a << 8
        begin; $a << 9
          raise; $a << 10
        rescue; $a << 11
          break; $a << 12
        end; $a << 13
      }; $a << 14
    end; $a << 15
  end; $a << 16
; $a << 17
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 2, 3, 4, 5, 6, 7, 8, 10, 13, 3, 4, 5, 6, 7, 8, 10, 13, 3, 4, 5, 6, 7, 8, 10, 13, 14, 15]}, %q{$a = []; begin; ; $a << 1
  i = 0; $a << 2
  while i<3; $a << 3
    i+=1; $a << 4
    j = 0; $a << 5
    while j<3; $a << 6
      j+=1; $a << 7
      begin; $a << 8
        raise; $a << 9
      ensure; $a << 10
        break; $a << 11
      end; $a << 12
    end; $a << 13
  end; $a << 14
; $a << 15
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 14, 6, 7, 8, 9, 11, 14, 6, 7, 8, 9, 11, 14, 15, 3, 4, 5, 6, 7, 8, 9, 11, 14, 6, 7, 8, 9, 11, 14, 6, 7, 8, 9, 11, 14, 15, 3, 4, 5, 6, 7, 8, 9, 11, 14, 6, 7, 8, 9, 11, 14, 6, 7, 8, 9, 11, 14, 15, 16, 17]}, %q{$a = []; begin; ; $a << 1
  i = 0; $a << 2
  while i<3; $a << 3
    i+=1; $a << 4
    j = 0; $a << 5
    while j<3; $a << 6
      j+=1; $a << 7
      1.times{; $a << 8
        begin; $a << 9
          raise; $a << 10
        ensure; $a << 11
          break; $a << 12
        end; $a << 13
      }; $a << 14
    end; $a << 15
  end; $a << 16
; $a << 17
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 2, 3, 5, 8, 9]}, %q{$a = []; begin; ; $a << 1
  while true; $a << 2
    begin; $a << 3
      break; $a << 4
    ensure; $a << 5
      break; $a << 6
    end; $a << 7
  end; $a << 8
; $a << 9
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 2, 3, 5, 99]}, %q{
$a = [];
begin; ; $a << 1
  while true; $a << 2
    begin; $a << 3
      break; $a << 4
    ensure; $a << 5
      raise; $a << 6
    end; $a << 7
  end; $a << 8
; $a << 9
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 2, 3, 4, 6, 8, 9, 10, 11]}, %q{$a = []; begin;  ; $a << 1
  begin; $a << 2
    [1,2].each do; $a << 3
      begin; $a << 4
        break; $a << 5
      ensure; $a << 6
      end; $a << 7
    end; $a << 8
  ensure; $a << 9
  end; $a << 10
; $a << 11
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 2, 4, 99]}, %q{$a = []; begin; ; $a << 1
  begin; $a << 2
    raise StandardError; $a << 3
  ensure; $a << 4
  end; $a << 5
; $a << 6
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 2, 3, 4]}, %q{$a = []; begin;  ; $a << 1
  begin; $a << 2
  ensure; $a << 3
  end ; $a << 4
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 2, 3, 5, 99]}, %q{$a = []; begin;  ; $a << 1
  [1,2].each do; $a << 2
    begin; $a << 3
      break; $a << 4
    ensure; $a << 5
      raise StandardError; $a << 6
    end; $a << 7
  end; $a << 8
; $a << 9
; rescue Exception; $a << 99; end; $a}
assert_equal %q{3}, %q{
  def m a, b
    a + b
  end
  m(1,
    while true
      break 2
    end
    )
}
assert_equal %q{4}, %q{
  def m a, b
    a + b
  end
  m(1,
    (i=0; while i<2
       i+=1
       class C
         next 2
       end
     end; 3)
    )
}
assert_equal %q{34}, %q{
  def m a, b
    a+b
  end
  m(1, 1.times{break 3}) +
  m(10, (1.times{next 3}; 20))
}
assert_equal %q{[1, 2, 3, 6, 7]}, %q{$a = []; begin; ; $a << 1
  3.times{; $a << 2
    class C; $a << 3
      break; $a << 4
    end; $a << 5
  }; $a << 6
; $a << 7
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 2, 3, 4, 8, 9]}, %q{$a = []; begin; ; $a << 1
  3.times{; $a << 2
    class A; $a << 3
      class B; $a << 4
        break; $a << 5
      end; $a << 6
    end; $a << 7
  }; $a << 8
; $a << 9
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 2, 3, 2, 3, 2, 3, 6, 7]}, %q{$a = []; begin; ; $a << 1
  3.times{; $a << 2
    class C; $a << 3
      next; $a << 4
    end; $a << 5
  }; $a << 6
; $a << 7
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 2, 3, 4, 2, 3, 4, 2, 3, 4, 8, 9]}, %q{$a = []; begin; ; $a << 1
  3.times{; $a << 2
    class C; $a << 3
      class D; $a << 4
        next; $a << 5
      end; $a << 6
    end; $a << 7
  }; $a << 8
; $a << 9
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 2, 3, 6, 7]}, %q{$a = []; begin; ; $a << 1
  while true; $a << 2
    class C; $a << 3
      break; $a << 4
    end; $a << 5
  end; $a << 6
; $a << 7
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 2, 3, 4, 8, 9]}, %q{$a = []; begin; ; $a << 1
  while true; $a << 2
    class C; $a << 3
      class D; $a << 4
        break; $a << 5
      end; $a << 6
    end; $a << 7
  end; $a << 8
; $a << 9
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 2, 3, 4, 5, 3, 4, 5, 3, 4, 5, 8, 9]}, %q{$a = []; begin; ; $a << 1
  i=0; $a << 2
  while i<3; $a << 3
    i+=1; $a << 4
    class C; $a << 5
      next 10; $a << 6
    end; $a << 7
  end; $a << 8
; $a << 9
; rescue Exception; $a << 99; end; $a}
assert_equal %q{1}, %q{
  1.times{
    while true
      class C
        begin
          break
        ensure
          break
        end
      end
    end
  }
}
assert_equal %q{[1, 2, 3, 5, 2, 3, 5, 7, 8]}, %q{$a = []; begin;  ; $a << 1
  [1,2].each do; $a << 2
    begin; $a << 3
      next; $a << 4
    ensure; $a << 5
    end; $a << 6
  end; $a << 7
; $a << 8
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 2, 6, 3, 5, 7, 8]}, %q{$a = []; begin;  ; $a << 1
  o = "test"; $a << 2
  def o.test(a); $a << 3
    return a; $a << 4
  ensure; $a << 5
  end; $a << 6
  o.test(123); $a << 7
; $a << 8
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 4, 7, 5, 8, 9]}, %q{$a = []; begin; ; $a << 1
  def m1 *args; $a << 2
    ; $a << 3
  end; $a << 4
  def m2; $a << 5
    m1(:a, :b, (return 1; :c)); $a << 6
  end; $a << 7
  m2; $a << 8
; $a << 9
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 8, 2, 3, 4, 5, 9, 10]}, %q{$a = []; begin; ; $a << 1
  def m(); $a << 2
    begin; $a << 3
      2; $a << 4
    ensure; $a << 5
      return 3; $a << 6
    end; $a << 7
  end; $a << 8
  m; $a << 9
; $a << 10
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 3, 11, 4, 5, 6, 7, 12, 13]}, %q{$a = []; begin; ; $a << 1
  def m2; $a << 2
  end; $a << 3
  def m(); $a << 4
    m2(begin; $a << 5
         2; $a << 6
       ensure; $a << 7
         return 3; $a << 8
       end); $a << 9
    4; $a << 10
  end; $a << 11
  m(); $a << 12
; $a << 13
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[1, 16, 2, 3, 4, 5, 6, 7, 10, 11, 17, 18]}, %q{$a = []; begin; ; $a << 1
  def m; $a << 2
    1; $a << 3
    1.times{; $a << 4
      2; $a << 5
      begin; $a << 6
        3; $a << 7
        return; $a << 8
        4; $a << 9
      ensure; $a << 10
        5; $a << 11
      end; $a << 12
      6; $a << 13
    }; $a << 14
    7; $a << 15
  end; $a << 16
  m(); $a << 17
; $a << 18
; rescue Exception; $a << 99; end; $a}
assert_equal %q{[:ok, :ok2, :last]}, %q{
  a = []
  i = 0
  begin
    while i < 1
      i+=1
      begin
        begin
          next
        ensure
          a << :ok
        end
      ensure
        a << :ok2
      end
    end
  ensure
    a << :last
  end
  a
}
assert_equal %q{[:ok, :ok2, :last]}, %q{
  a = []
  i = 0
  begin
    while i < 1
      i+=1
      begin
        begin
          break
        ensure
          a << :ok
        end
      ensure
        a << :ok2
      end
    end
  ensure
    a << :last
  end
  a
}
assert_equal %q{[:ok, :ok2, :last]}, %q{
  a = []
  i = 0
  begin
    while i < 1
      if i>0
        break
      end
      i+=1
      begin
        begin
          redo
        ensure
          a << :ok
        end
      ensure
        a << :ok2
      end
    end
  ensure
    a << :last
  end
  a
}
assert_equal %Q{ENSURE\n}, %q{
  def test
    while true
      return
    end
  ensure
    puts("ENSURE")
  end
  test
}, '[ruby-dev:37967]'

[['[ruby-core:28129]', %q{
  class Bug2728
    include Enumerable
    define_method(:dynamic_method) do
      "dynamically defined method"
    end
    def each
      begin
        yield :foo
      ensure
        dynamic_method
      end
    end
  end
  e = Bug2728.new
}],
 ['[ruby-core:28132]', %q{
  class Bug2729
    include Enumerable
    def each
      begin
        yield :foo
      ensure
        proc {}.call
      end
    end
  end
  e = Bug2729.new
}],
 ['[ruby-core:39125]', %q{
  class Bug5234
    include Enumerable
    def each
      begin
        yield :foo
      ensure
        proc
      end
    end
  end
  e = Bug5234.new
}]].each do |bug, src|
  assert_equal "foo", src + %q{e.detect {true}}, bug
  assert_equal "true", src + %q{e.any? {true}}, bug
  assert_equal "false", src + %q{e.all? {false}}, bug
  assert_equal "true", src + %q{e.include?(:foo)}, bug
end
