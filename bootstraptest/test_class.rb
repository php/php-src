# class
assert_equal 'true',    %q( class C; end
                            Object.const_defined?(:C) )
assert_equal 'Class',   %q( class C; end
                            C.class )
assert_equal 'C',       %q( class C; end
                            C.name )
assert_equal 'C',       %q( class C; end
                            C.new.class )
assert_equal 'C',       %q( class C; end
                            C.new.class.name )
assert_equal 'Class',   %q( class C; end
                            C.new.class.class )

# inherited class
assert_equal 'true',    %q( class A; end
                            class C < A; end
                            Object.const_defined?(:C) )
assert_equal 'Class',   %q( class A; end
                            class C < A; end
                            C.class )
assert_equal 'C',       %q( class A; end
                            class C < A; end
                            C.name )
assert_equal 'C',       %q( class A; end
                            class C < A; end
                            C.new.class )
assert_equal 'C',       %q( class A; end
                            class C < A; end
                            C.new.class.name )
assert_equal 'Class',   %q( class A; end
                            class C < A; end
                            C.new.class.class )

# module
assert_equal 'true',    %q( module M; end
                            Object.const_defined?(:M) )
assert_equal 'Module',  %q( module M; end
                            M.class )
assert_equal 'M',       %q( module M; end
                            M.name )
assert_equal 'C',       %q( module M; end
                            class C; include M; end
                            C.new.class )

# nested class
assert_equal 'A::B',    %q( class A; end
                            class A::B; end
                            A::B )
assert_equal 'A::B',    %q( class A; end
                            class A::B; end
                            A::B.name )
assert_equal 'A::B',    %q( class A; end
                            class A::B; end
                            A::B.new.class )
assert_equal 'Class',   %q( class A; end
                            class A::B; end
                            A::B.new.class.class )
assert_equal 'A::B::C', %q( class A; end
                            class A::B; end
                            class A::B::C; end
                            A::B::C )
assert_equal 'A::B::C', %q( class A; end
                            class A::B; end
                            class A::B::C; end
                            A::B::C.name )
assert_equal 'Class',   %q( class A; end
                            class A::B; end
                            class A::B::C; end
                            A::B::C.class )
assert_equal 'A::B::C', %q( class A; end
                            class A::B; end
                            class A::B::C; end
                            A::B::C.new.class )
assert_equal 'Class',   %q( class A; end
                            class A::B; end
                            class A::B::C; end
                            A::B::C.new.class.class )
assert_equal 'A::B2',   %q( class A; end
                            class A::B; end
                            class A::B2 < A::B; end
                            A::B2 )
assert_equal 'Class',   %q( class A; end
                            class A::B; end
                            class A::B2 < A::B; end
                            A::B2.class )

# reopen
assert_equal 'true',    %q( class C; end;  c1 = ::C
                            class C; end;  c2 = ::C
                            c1.equal?(c2) )
assert_equal '1',       %q( class C; end
                            class A; end
                            begin class C < A; end; rescue TypeError; 1 end )
assert_equal '1',       %q( class C; end
                            begin module C; end; rescue TypeError; 1 end )
assert_equal '1',       %q( C = 1   # [yarv-dev:782]
                            begin class C; end; rescue TypeError; 1 end )
assert_equal '1',       %q( C = 1   # [yarv-dev:800]
                            begin module C; end; rescue TypeError; 1 end )

# colon2, colon3
assert_equal '1',       %q( class A; end;  A::C = 1;  A::C )
assert_equal '1',       %q( A = 7;  begin A::C = 7; rescue TypeError; 1 end )
assert_equal '1',       %q( begin 7::C = 7; rescue TypeError; 1 end )
assert_equal 'C',       %q( class A; class ::C; end end;  C )
assert_equal 'Class',   %q( class A; class ::C; end end;  C.class )
assert_equal 'OK',      %q( class A; ::C = "OK"; end;  C )
assert_equal 'String',  %q( class A; ::C = "OK"; end;  C.class )

# class/module dup
assert_equal 'Class',   %q( class C; end;  C.dup.class )
assert_equal 'Module',  %q( module M; end;  M.dup.class )


assert_equal "ok", %q{
  module Foo
  end

  begin
    def foo(&b)
      Foo.module_eval &b
    end
    foo{
      def bar
      end
    }
    bar()
  rescue NameError
    :ok
  end
}, '[ruby-core:14378]'

assert_equal '3', %q{
  $i = 0
  class C
    def self.const_missing *args
      $i+=1
    end
  end

  3.times{
    C::FOO
  }
  $i
}

assert_match /::C\z/, %q{
  c = nil
  Module.new{|m| c = class m::C; name; end}
  c
}, '[ruby-dev:38456]'

assert_normal_exit %q{
  s = Symbol.dup
  class << s
  end
  s.allocate.to_s
}, '[ruby-core:30843]'
