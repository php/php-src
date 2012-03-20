#! /usr/bin/env ruby
# -*- coding: us-ascii -*-

$testnum=0
$ntest=0
$failed = 0

def test_check(what)
  STDERR.print "\nsample/test.rb:#{what} "
  $what = what
  $testnum = 0
end

def test_ok(cond,n=1)
  $testnum+=1
  $ntest+=1
  where = (st = caller(n)) ? st[0] : "caller error! (n=#{n}, trace=#{caller(0).join(', ')}"
  if cond
    STDERR.print "."
    printf "ok %d (%s)\n", $testnum, where
  else
    STDERR.print "F"
    printf "not ok %s %d -- %s\n", $what, $testnum, where
    $failed+=1
  end
  STDOUT.flush
  STDERR.flush
end

# make sure conditional operators work

test_check "assignment"

a=[]; a[0] ||= "bar";
test_ok(a[0] == "bar")
h={}; h["foo"] ||= "bar";
test_ok(h["foo"] == "bar")

aa = 5
aa ||= 25
test_ok(aa == 5)
bb ||= 25
test_ok(bb == 25)
cc &&=33
test_ok(cc == nil)
cc = 5
cc &&=44
test_ok(cc == 44)

a = nil; test_ok(a == nil)
a = 1; test_ok(a == 1)
a = []; test_ok(a == [])
a = [1]; test_ok(a == [1])
a = [nil]; test_ok(a == [nil])
a = [[]]; test_ok(a == [[]])
a = [1,2]; test_ok(a == [1,2])
a = [*[]]; test_ok(a == [])
a = [*[1]]; test_ok(a == [1])
a = [*[1,2]]; test_ok(a == [1,2])

a = *[]; test_ok(a == [])
a = *[1]; test_ok(a == [1])
a = *[nil]; test_ok(a == [nil])
a = *[[]]; test_ok(a == [[]])
a = *[1,2]; test_ok(a == [1,2])
a = *[*[]]; test_ok(a == [])
a = *[*[1]]; test_ok(a == [1])
a = *[*[1,2]]; test_ok(a == [1,2])

a, = nil; test_ok(a == nil)
a, = 1; test_ok(a == 1)
a, = []; test_ok(a == nil)
a, = [1]; test_ok(a == 1)
a, = [nil]; test_ok(a == nil)
a, = [[]]; test_ok(a == [])
a, = 1,2; test_ok(a == 1)
a, = [1,2]; test_ok(a == 1)
a, = [*[]]; test_ok(a == nil)
a, = [*[1]]; test_ok(a == 1)
a, = *[1,2]; test_ok(a == 1)
a, = [*[1,2]]; test_ok(a == 1)

a, = *[]; test_ok(a == nil)
a, = *[1]; test_ok(a == 1)
a, = *[nil]; test_ok(a == nil)
a, = *[[]]; test_ok(a == [])
a, = *[1,2]; test_ok(a == 1)
a, = *[*[]]; test_ok(a == nil)
a, = *[*[1]]; test_ok(a == 1)
a, = *[*[1,2]]; test_ok(a == 1)

*a = nil; test_ok(a == [nil])
*a = 1; test_ok(a == [1])
*a = []; test_ok(a == [])
*a = [1]; test_ok(a == [1])
*a = [nil]; test_ok(a == [nil])
*a = [[]]; test_ok(a == [[]])
*a = [1,2]; test_ok(a == [1,2])
*a = [*[]]; test_ok(a == [])
*a = [*[1]]; test_ok(a == [1])
*a = [*[1,2]]; test_ok(a == [1,2])

*a = *[]; test_ok(a == [])
*a = *[1]; test_ok(a == [1])
*a = *[nil]; test_ok(a == [nil])
*a = *[[]]; test_ok(a == [[]])
*a = *[1,2]; test_ok(a == [1,2])
*a = *[*[]]; test_ok(a == [])
*a = *[*[1]]; test_ok(a == [1])
*a = *[*[1,2]]; test_ok(a == [1,2])

a,b,*c = nil; test_ok([a,b,c] == [nil,nil,[]])
a,b,*c = 1; test_ok([a,b,c] == [1,nil,[]])
a,b,*c = []; test_ok([a,b,c] == [nil,nil,[]])
a,b,*c = [1]; test_ok([a,b,c] == [1,nil,[]])
a,b,*c = [nil]; test_ok([a,b,c] == [nil,nil,[]])
a,b,*c = [[]]; test_ok([a,b,c] == [[],nil,[]])
a,b,*c = [1,2]; test_ok([a,b,c] == [1,2,[]])
a,b,*c = [*[]]; test_ok([a,b,c] == [nil,nil,[]])
a,b,*c = [*[1]]; test_ok([a,b,c] == [1,nil,[]])
a,b,*c = [*[1,2]]; test_ok([a,b,c] == [1,2,[]])

a,b,*c = *[]; test_ok([a,b,c] == [nil,nil,[]])
a,b,*c = *[1]; test_ok([a,b,c] == [1,nil,[]])
a,b,*c = *[nil]; test_ok([a,b,c] == [nil,nil,[]])
a,b,*c = *[[]]; test_ok([a,b,c] == [[],nil,[]])
a,b,*c = *[1,2]; test_ok([a,b,c] == [1,2,[]])
a,b,*c = *[*[]]; test_ok([a,b,c] == [nil,nil,[]])
a,b,*c = *[*[1]]; test_ok([a,b,c] == [1,nil,[]])
a,b,*c = *[*[1,2]]; test_ok([a,b,c] == [1,2,[]])

def f; yield nil; end; f {|a| test_ok(a == nil)}
def f; yield 1; end; f {|a| test_ok(a == 1)}
def f; yield []; end; f {|a| test_ok(a == [])}
def f; yield [1]; end; f {|a| test_ok(a == [1])}
def f; yield [nil]; end; f {|a| test_ok(a == [nil])}
def f; yield [[]]; end; f {|a| test_ok(a == [[]])}
def f; yield [*[]]; end; f {|a| test_ok(a == [])}
def f; yield [*[1]]; end; f {|a| test_ok(a == [1])}
def f; yield [*[1,2]]; end; f {|a| test_ok(a == [1,2])}
def f; yield *[]; end; f {|a| test_ok(a == nil)}
def f; yield *[1]; end; f {|a| test_ok(a == 1)}
def f; yield *[nil]; end; f {|a| test_ok(a == nil)}
def f; yield *[[]]; end; f {|a| test_ok(a == [])}
def f; yield *[*[]]; end; f {|a| test_ok(a == nil)}
def f; yield *[*[1]]; end; f {|a| test_ok(a == 1)}
def f; yield *[*[1,2]]; end; f {|a| test_ok(a == 1)}

def f; yield; end; f {|a,| test_ok(a == nil)}
def f; yield nil; end; f {|a,| test_ok(a == nil)}
def f; yield 1; end; f {|a,| test_ok(a == 1)}
def f; yield []; end; f {|a,| test_ok(a == nil)}
def f; yield [1]; end; f {|a,| test_ok(a == 1)}
def f; yield [nil]; end; f {|a,| test_ok(a == nil)}
def f; yield [[]]; end; f {|a,| test_ok(a == [])}
def f; yield [*[]]; end; f {|a,| test_ok(a == nil)}
def f; yield [*[1]]; end; f {|a,| test_ok(a == 1)}
def f; yield [*[1,2]]; end; f {|a,| test_ok(a == 1)}

def f; yield *[]; end; f {|a,| test_ok(a == nil)}
def f; yield *[1]; end; f {|a,| test_ok(a == 1)}
def f; yield *[nil]; end; f {|a,| test_ok(a == nil)}
def f; yield *[[]]; end; f {|a,| test_ok(a == nil)}
def f; yield *[*[]]; end; f {|a,| test_ok(a == nil)}
def f; yield *[*[1]]; end; f {|a,| test_ok(a == 1)}
def f; yield *[*[1,2]]; end; f {|a,| test_ok(a == 1)}

def f; yield; end; f {|*a| test_ok(a == [])}
def f; yield nil; end; f {|*a| test_ok(a == [nil])}
def f; yield 1; end; f {|*a| test_ok(a == [1])}
def f; yield []; end; f {|*a| test_ok(a == [[]])}
def f; yield [1]; end; f {|*a| test_ok(a == [[1]])}
def f; yield [nil]; end; f {|*a| test_ok(a == [[nil]])}
def f; yield [[]]; end; f {|*a| test_ok(a == [[[]]])}
def f; yield [1,2]; end; f {|*a| test_ok(a == [[1,2]])}
def f; yield [*[]]; end; f {|*a| test_ok(a == [[]])}
def f; yield [*[1]]; end; f {|*a| test_ok(a == [[1]])}
def f; yield [*[1,2]]; end; f {|*a| test_ok(a == [[1,2]])}

def f; yield *[]; end; f {|*a| test_ok(a == [])}
def f; yield *[1]; end; f {|*a| test_ok(a == [1])}
def f; yield *[nil]; end; f {|*a| test_ok(a == [nil])}
def f; yield *[[]]; end; f {|*a| test_ok(a == [[]])}
def f; yield *[*[]]; end; f {|*a| test_ok(a == [])}
def f; yield *[*[1]]; end; f {|*a| test_ok(a == [1])}
def f; yield *[*[1,2]]; end; f {|*a| test_ok(a == [1,2])}

def f; yield; end; f {|a,b,*c| test_ok([a,b,c] == [nil,nil,[]])}
def f; yield nil; end; f {|a,b,*c| test_ok([a,b,c] == [nil,nil,[]])}
def f; yield 1; end; f {|a,b,*c| test_ok([a,b,c] == [1,nil,[]])}
def f; yield []; end; f {|a,b,*c| test_ok([a,b,c] == [nil,nil,[]])}
def f; yield [1]; end; f {|a,b,*c| test_ok([a,b,c] == [1,nil,[]])}
def f; yield [nil]; end; f {|a,b,*c| test_ok([a,b,c] == [nil,nil,[]])}
def f; yield [[]]; end; f {|a,b,*c| test_ok([a,b,c] == [[],nil,[]])}
def f; yield [*[]]; end; f {|a,b,*c| test_ok([a,b,c] == [nil,nil,[]])}
def f; yield [*[1]]; end; f {|a,b,*c| test_ok([a,b,c] == [1,nil,[]])}
def f; yield [*[1,2]]; end; f {|a,b,*c| test_ok([a,b,c] == [1,2,[]])}

def f; yield *[]; end; f {|a,b,*c| test_ok([a,b,c] == [nil,nil,[]])}
def f; yield *[1]; end; f {|a,b,*c| test_ok([a,b,c] == [1,nil,[]])}
def f; yield *[nil]; end; f {|a,b,*c| test_ok([a,b,c] == [nil,nil,[]])}
def f; yield *[[]]; end; f {|a,b,*c| test_ok([a,b,c] == [nil,nil,[]])}
def f; yield *[*[]]; end; f {|a,b,*c| test_ok([a,b,c] == [nil,nil,[]])}
def f; yield *[*[1]]; end; f {|a,b,*c| test_ok([a,b,c] == [1,nil,[]])}
def f; yield *[*[1,2]]; end; f {|a,b,*c| test_ok([a,b,c] == [1,2,[]])}

def r; return; end; a = r(); test_ok(a == nil)
def r; return nil; end; a = r(); test_ok(a == nil)
def r; return 1; end; a = r(); test_ok(a == 1)
def r; return []; end; a = r(); test_ok(a == [])
def r; return [1]; end; a = r(); test_ok(a == [1])
def r; return [nil]; end; a = r(); test_ok(a == [nil])
def r; return [[]]; end; a = r(); test_ok(a == [[]])
def r; return [*[]]; end; a = r(); test_ok(a == [])
def r; return [*[1]]; end; a = r(); test_ok(a == [1])
def r; return [*[1,2]]; end; a = r(); test_ok(a == [1,2])

def r; return *[]; end; a = r(); test_ok(a == [])
def r; return *[1]; end; a = r(); test_ok(a == [1])
def r; return *[nil]; end; a = r(); test_ok(a == [nil])
def r; return *[[]]; end; a = r(); test_ok(a == [[]])
def r; return *[*[]]; end; a = r(); test_ok(a == [])
def r; return *[*[1]]; end; a = r(); test_ok(a == [1])
def r; return *[*[1,2]]; end; a = r(); test_ok(a == [1,2])

def r; return *[[]]; end; a = *r(); test_ok(a == [[]])
def r; return *[*[1,2]]; end; a = *r(); test_ok(a == [1,2])

def r; return; end; *a = r(); test_ok(a == [nil])
def r; return nil; end; *a = r(); test_ok(a == [nil])
def r; return 1; end; *a = r(); test_ok(a == [1])
def r; return []; end; *a = r(); test_ok(a == [])
def r; return [1]; end; *a = r(); test_ok(a == [1])
def r; return [nil]; end; *a = r(); test_ok(a == [nil])
def r; return [[]]; end; *a = r(); test_ok(a == [[]])
def r; return [1,2]; end; *a = r(); test_ok(a == [1,2])
def r; return [*[]]; end; *a = r(); test_ok(a == [])
def r; return [*[1]]; end; *a = r(); test_ok(a == [1])
def r; return [*[1,2]]; end; *a = r(); test_ok(a == [1,2])

def r; return *[]; end; *a = r(); test_ok(a == [])
def r; return *[1]; end; *a = r(); test_ok(a == [1])
def r; return *[nil]; end; *a = r(); test_ok(a == [nil])
def r; return *[[]]; end; *a = r(); test_ok(a == [[]])
def r; return *[1,2]; end; *a = r(); test_ok(a == [1,2])
def r; return *[*[]]; end; *a = r(); test_ok(a == [])
def r; return *[*[1]]; end; *a = r(); test_ok(a == [1])
def r; return *[*[1,2]]; end; *a = r(); test_ok(a == [1,2])

def r; return *[[]]; end; *a = *r(); test_ok(a == [[]])
def r; return *[1,2]; end; *a = *r(); test_ok(a == [1,2])
def r; return *[*[1,2]]; end; *a = *r(); test_ok(a == [1,2])

def r; return; end; a,b,*c = r(); test_ok([a,b,c] == [nil,nil,[]])
def r; return nil; end; a,b,*c = r(); test_ok([a,b,c] == [nil,nil,[]])
def r; return 1; end; a,b,*c = r(); test_ok([a,b,c] == [1,nil,[]])
def r; return []; end; a,b,*c = r(); test_ok([a,b,c] == [nil,nil,[]])
def r; return [1]; end; a,b,*c = r(); test_ok([a,b,c] == [1,nil,[]])
def r; return [nil]; end; a,b,*c = r(); test_ok([a,b,c] == [nil,nil,[]])
def r; return [[]]; end; a,b,*c = r(); test_ok([a,b,c] == [[],nil,[]])
def r; return [1,2]; end; a,b,*c = r(); test_ok([a,b,c] == [1,2,[]])
def r; return [*[]]; end; a,b,*c = r(); test_ok([a,b,c] == [nil,nil,[]])
def r; return [*[1]]; end; a,b,*c = r(); test_ok([a,b,c] == [1,nil,[]])
def r; return [*[1,2]]; end; a,b,*c = r(); test_ok([a,b,c] == [1,2,[]])

def r; return *[]; end; a,b,*c = r(); test_ok([a,b,c] == [nil,nil,[]])
def r; return *[1]; end; a,b,*c = r(); test_ok([a,b,c] == [1,nil,[]])
def r; return *[nil]; end; a,b,*c = r(); test_ok([a,b,c] == [nil,nil,[]])
def r; return *[[]]; end; a,b,*c = r(); test_ok([a,b,c] == [[],nil,[]])
def r; return *[1,2]; end; a,b,*c = r(); test_ok([a,b,c] == [1,2,[]])
def r; return *[*[]]; end; a,b,*c = r(); test_ok([a,b,c] == [nil,nil,[]])
def r; return *[*[1]]; end; a,b,*c = r(); test_ok([a,b,c] == [1,nil,[]])
def r; return *[*[1,2]]; end; a,b,*c = r(); test_ok([a,b,c] == [1,2,[]])

f = lambda {|r,| test_ok([] == r)}
f.call([], *[])

f = lambda {|r,*l| test_ok([] == r); test_ok([1] == l)}
f.call([], *[1])

f = lambda{|x| x}
test_ok(f.call(42) == 42)
test_ok(f.call([42]) == [42])
test_ok(f.call([[42]]) == [[42]])
test_ok(f.call([42,55]) == [42,55])

f = lambda{|x,| x}
test_ok(f.call(42) == 42)
test_ok(f.call([42]) == [42])
test_ok(f.call([[42]]) == [[42]])
test_ok(f.call([42,55]) == [42,55])

f = lambda{|*x| x}
test_ok(f.call(42) == [42])
test_ok(f.call([42]) == [[42]])
test_ok(f.call([[42]]) == [[[42]]])
test_ok(f.call([42,55]) == [[42,55]])
test_ok(f.call(42,55) == [42,55])

f = lambda { |a, b=42, *c| [a,b,c] }
test_ok(f.call(1      ) == [1,42,[  ]] )
test_ok(f.call(1,43   ) == [1,43,[  ]] )
test_ok(f.call(1,43,44) == [1,43,[44]] )

f = lambda { |a, b=(a|16), *c, &block| [a,b,c,block&&block[]] }
test_ok(f.call(8      )     == [8,24,[  ],nil] )
test_ok(f.call(8,43   )     == [8,43,[  ],nil] )
test_ok(f.call(8,43,44)     == [8,43,[44],nil] )
test_ok(f.call(8      ){45} == [8,24,[  ],45 ] )
test_ok(f.call(8,43   ){45} == [8,43,[  ],45 ] )
test_ok(f.call(8,43,44){45} == [8,43,[44],45 ] )

f = lambda { |a, b=42, *c, d| [a,b,c,d] }
test_ok(f.call(1      ,99) == [1,42,[  ],99] )
test_ok(f.call(1,43   ,99) == [1,43,[  ],99] )
test_ok(f.call(1,43,44,99) == [1,43,[44],99] )

f = lambda { |a, b=(a|16), &block| [a,b,block&&block[]] }
test_ok(f.call(8   )     == [8,24,nil] )
test_ok(f.call(8,43)     == [8,43,nil] )
test_ok(f.call(8,43)     == [8,43,nil] )
test_ok(f.call(8   ){45} == [8,24,45 ] )
test_ok(f.call(8,43){45} == [8,43,45 ] )
test_ok(f.call(8,43){45} == [8,43,45 ] )

f = lambda { |a, b=42, d| [a,b,d] }
test_ok(f.call(1   ,99) == [1,42,99] )
test_ok(f.call(1,43,99) == [1,43,99] )
test_ok(f.call(1,43,99) == [1,43,99] )

f = lambda { |b=42, *c, &block| [b,c,block&&block[]] }
test_ok(f.call(     )     == [42,[  ],nil] )
test_ok(f.call(43   )     == [43,[  ],nil] )
test_ok(f.call(43,44)     == [43,[44],nil] )
test_ok(f.call(     ){45} == [42,[  ],45 ] )
test_ok(f.call(43   ){45} == [43,[  ],45 ] )
test_ok(f.call(43,44){45} == [43,[44],45 ] )

f = lambda { |b=42, *c, d| [b,c,d] }
test_ok(f.call(      99) == [42,[  ],99] )
test_ok(f.call(43   ,99) == [43,[  ],99] )
test_ok(f.call(43,44,99) == [43,[44],99] )

f = lambda { |b=42, &block| [b,block&&block[]] }
test_ok(f.call(  )     == [42,nil] )
test_ok(f.call(43)     == [43,nil] )
test_ok(f.call(43)     == [43,nil] )
test_ok(f.call(  ){45} == [42,45 ] )
test_ok(f.call(43){45} == [43,45 ] )
test_ok(f.call(43){45} == [43,45 ] )

f = lambda { |b=42, d| [b,d] }
test_ok(f.call(   99) == [42,99] )
test_ok(f.call(43,99) == [43,99] )
test_ok(f.call(43,99) == [43,99] )


a,=*[1]
test_ok(a == 1)
a,=*[[1]]
test_ok(a == [1])
a,=*[[[1]]]
test_ok(a == [[1]])

x, (y, z) = 1, 2, 3
test_ok([1,2,nil] == [x,y,z])
x, (y, z) = 1, [2,3]
test_ok([1,2,3] == [x,y,z])
x, (y, z) = 1, [2]
test_ok([1,2,nil] == [x,y,z])

a = loop do break; end; test_ok(a == nil)
a = loop do break nil; end; test_ok(a == nil)
a = loop do break 1; end; test_ok(a == 1)
a = loop do break []; end; test_ok(a == [])
a = loop do break [1]; end; test_ok(a == [1])
a = loop do break [nil]; end; test_ok(a == [nil])
a = loop do break [[]]; end; test_ok(a == [[]])
a = loop do break [*[]]; end; test_ok(a == [])
a = loop do break [*[1]]; end; test_ok(a == [1])
a = loop do break [*[1,2]]; end; test_ok(a == [1,2])

a = loop do break *[]; end; test_ok(a == [])
a = loop do break *[1]; end; test_ok(a == [1])
a = loop do break *[nil]; end; test_ok(a == [nil])
a = loop do break *[[]]; end; test_ok(a == [[]])
a = loop do break *[*[]]; end; test_ok(a == [])
a = loop do break *[*[1]]; end; test_ok(a == [1])
a = loop do break *[*[1,2]]; end; test_ok(a == [1,2])

*a = loop do break; end; test_ok(a == [nil])
*a = loop do break nil; end; test_ok(a == [nil])
*a = loop do break 1; end; test_ok(a == [1])
*a = loop do break []; end; test_ok(a == [])
*a = loop do break [1]; end; test_ok(a == [1])
*a = loop do break [nil]; end; test_ok(a == [nil])
*a = loop do break [[]]; end; test_ok(a == [[]])
*a = loop do break [1,2]; end; test_ok(a == [1,2])
*a = loop do break [*[]]; end; test_ok(a == [])
*a = loop do break [*[1]]; end; test_ok(a == [1])
*a = loop do break [*[1,2]]; end; test_ok(a == [1,2])

*a = loop do break *[]; end; test_ok(a == [])
*a = loop do break *[1]; end; test_ok(a == [1])
*a = loop do break *[nil]; end; test_ok(a == [nil])
*a = loop do break *[[]]; end; test_ok(a == [[]])
*a = loop do break *[1,2]; end; test_ok(a == [1,2])
*a = loop do break *[*[]]; end; test_ok(a == [])
*a = loop do break *[*[1]]; end; test_ok(a == [1])
*a = loop do break *[*[1,2]]; end; test_ok(a == [1,2])

*a = *loop do break *[[]]; end; test_ok(a == [[]])
*a = *loop do break *[1,2]; end; test_ok(a == [1,2])
*a = *loop do break *[*[1,2]]; end; test_ok(a == [1,2])

a,b,*c = loop do break; end; test_ok([a,b,c] == [nil,nil,[]])
a,b,*c = loop do break nil; end; test_ok([a,b,c] == [nil,nil,[]])
a,b,*c = loop do break 1; end; test_ok([a,b,c] == [1,nil,[]])
a,b,*c = loop do break []; end; test_ok([a,b,c] == [nil,nil,[]])
a,b,*c = loop do break [1]; end; test_ok([a,b,c] == [1,nil,[]])
a,b,*c = loop do break [nil]; end; test_ok([a,b,c] == [nil,nil,[]])
a,b,*c = loop do break [[]]; end; test_ok([a,b,c] == [[],nil,[]])
a,b,*c = loop do break [1,2]; end; test_ok([a,b,c] == [1,2,[]])
a,b,*c = loop do break [*[]]; end; test_ok([a,b,c] == [nil,nil,[]])
a,b,*c = loop do break [*[1]]; end; test_ok([a,b,c] == [1,nil,[]])
a,b,*c = loop do break [*[1,2]]; end; test_ok([a,b,c] == [1,2,[]])

a,b,*c = loop do break *[]; end; test_ok([a,b,c] == [nil,nil,[]])
a,b,*c = loop do break *[1]; end; test_ok([a,b,c] == [1,nil,[]])
a,b,*c = loop do break *[nil]; end; test_ok([a,b,c] == [nil,nil,[]])
a,b,*c = loop do break *[[]]; end; test_ok([a,b,c] == [[],nil,[]])
a,b,*c = loop do break *[1,2]; end; test_ok([a,b,c] == [1,2,[]])
a,b,*c = loop do break *[*[]]; end; test_ok([a,b,c] == [nil,nil,[]])
a,b,*c = loop do break *[*[1]]; end; test_ok([a,b,c] == [1,nil,[]])
a,b,*c = loop do break *[*[1,2]]; end; test_ok([a,b,c] == [1,2,[]])

def r(val); a = yield(); test_ok(a == val, 2); end
r(nil){next}
r(nil){next nil}
r(1){next 1}
r([]){next []}
r([1]){next [1]}
r([nil]){next [nil]}
r([[]]){next [[]]}
r([]){next [*[]]}
r([1]){next [*[1]]}
r([1,2]){next [*[1,2]]}

r([]){next *[]}
r([1]){next *[1]}
r([nil]){next *[nil]}
r([[]]){next *[[]]}
r([]){next *[*[]]}
r([1]){next *[*[1]]}
r([1,2]){next *[*[1,2]]}

def r(val); *a = yield(); test_ok(a == val, 2); end
r([nil]){next}
r([nil]){next nil}
r([1]){next 1}
r([]){next []}
r([1]){next [1]}
r([nil]){next [nil]}
r([[]]){next [[]]}
r([1,2]){next [1,2]}
r([]){next [*[]]}
r([1]){next [*[1]]}
r([1,2]){next [*[1,2]]}

def r(val); *a = *yield(); test_ok(a == val, 2); end
r([[]]){next *[[]]}
r([1,2]){next *[1,2]}
r([1,2]){next *[*[1,2]]}

def r(val); a,b,*c = yield(); test_ok([a,b,c] == val, 2); end
r([nil,nil,[]]){next}
r([nil,nil,[]]){next nil}
r([1,nil,[]]){next 1}
r([nil,nil,[]]){next []}
r([1,nil,[]]){next [1]}
r([nil,nil,[]]){next [nil]}
r([[],nil,[]]){next [[]]}
r([1,2,[]]){next [1,2]}
r([nil,nil,[]]){next [*[]]}
r([1,nil,[]]){next [*[1]]}
r([1,2,[]]){next [*[1,2]]}

def r(val); a,b,*c = *yield(); test_ok([a,b,c] == val, 2); end
r([[],nil,[]]){next *[[]]}
r([1,2,[]]){next *[1,2]}
r([1,2,[]]){next *[*[1,2]]}

test_check "condition"

$x = '0';

$x == $x && test_ok(true)
$x != $x && test_ok(false)
$x == $x || test_ok(false)
$x != $x || test_ok(true)

# first test to see if we can run the tests.

test_check "if/unless";

$x = 'test';
test_ok(if $x == $x then true else false end)
$bad = false
unless $x == $x
  $bad = true
end
test_ok(!$bad)
test_ok(unless $x != $x then true else false end)

test_check "case"

case 5
when 1, 2, 3, 4, 6, 7, 8
  test_ok(false)
when 5
  test_ok(true)
end

case 5
when 5
  test_ok(true)
when 1..10
  test_ok(false)
end

case 5
when 1..10
  test_ok(true)
else
  test_ok(false)
end

case 5
when 5
  test_ok(true)
else
  test_ok(false)
end

case "foobar"
when /^f.*r$/
  test_ok(true)
else
  test_ok(false)
end

test_check "while/until";

tmp = open("while_tmp", "w")
tmp.print "tvi925\n";
tmp.print "tvi920\n";
tmp.print "vt100\n";
tmp.print "Amiga\n";
tmp.print "paper\n";
tmp.close

# test break

tmp = open("while_tmp", "r")
test_ok(tmp.kind_of?(File))

while line = tmp.gets()
  break if /vt100/ =~ line
end

test_ok(!tmp.eof? && /vt100/ =~ line)
tmp.close

# test next
$bad = false
tmp = open("while_tmp", "r")
while line = tmp.gets()
  next if /vt100/ =~ line
  $bad = 1 if /vt100/ =~ line
end
test_ok(!(!tmp.eof? || /vt100/ =~ line || $bad))
tmp.close

# test redo
$bad = false
tmp = open("while_tmp", "r")
while line = tmp.gets()
  lastline = line
  line = line.gsub(/vt100/, 'VT100')
  if lastline != line
    line.gsub!('VT100', 'Vt100')
    redo
  end
  $bad = 1 if /vt100/ =~ line
  $bad = 1 if /VT100/ =~ line
end
test_ok(tmp.eof? && !$bad)
tmp.close

sum=0
for i in 1..10
  sum += i
  i -= 1
  if i > 0
    redo
  end
end
test_ok(sum == 220)

# test interval
$bad = false
tmp = open("while_tmp", "r")
while line = tmp.gets()
  break if 3
  case line
  when /vt100/, /Amiga/, /paper/
    $bad = true
  end
end
test_ok(!$bad)
tmp.close

File.unlink "while_tmp" or `/bin/rm -f "while_tmp"`
test_ok(!File.exist?("while_tmp"))

i = 0
until i>4
  i+=1
end
test_ok(i>4)


# exception handling
test_check "exception";

begin
  raise "this must be handled"
  test_ok(false)
rescue
  test_ok(true)
end

$bad = true
begin
  raise "this must be handled no.2"
rescue
  if $bad
    $bad = false
    retry
    test_ok(false)
  end
end
test_ok(true)

# exception in rescue clause
$string = "this must be handled no.3"
begin
  begin
    raise "exception in rescue clause"
  rescue
    raise $string
  end
  test_ok(false)
rescue => e
  test_ok($! == e)
  test_ok(e.message == $string)
  test_ok(e != $string)
end

# exception in ensure clause
begin
  begin
    raise "this must be handled no.4"
  ensure
    raise "exception in ensure clause"
  end
  test_ok(false)
rescue
  test_ok(true)
end

$bad = true
begin
  begin
    raise "this must be handled no.5"
  ensure
    $bad = false
  end
rescue
end
test_ok(!$bad)

$bad = true
begin
  begin
    raise "this must be handled no.6"
  ensure
    $bad = false
  end
rescue
end
test_ok(!$bad)

$bad = true
while true
  begin
    break
  ensure
    $bad = false
  end
end
test_ok(!$bad)

test_ok(catch(:foo) {
     loop do
       loop do
	 throw :foo, true
	 break
       end
       break
       test_ok(false)			# should no reach here
     end
     false
   })

test_check "array"
test_ok([1, 2] + [3, 4] == [1, 2, 3, 4])
test_ok([1, 2] * 2 == [1, 2, 1, 2])
test_ok([1, 2] * ":" == "1:2")

test_ok([1, 2].hash == [1, 2].hash)

test_ok([1,2,3] & [2,3,4] == [2,3])
test_ok([1,2,3] | [2,3,4] == [1,2,3,4])
test_ok([1,2,3] - [2,3] == [1])

$x = [0, 1, 2, 3, 4, 5]
test_ok($x[2] == 2)
test_ok($x[1..3] == [1, 2, 3])
test_ok($x[1,3] == [1, 2, 3])

$x[0, 2] = 10
test_ok($x[0] == 10 && $x[1] == 2)

$x[0, 0] = -1
test_ok($x[0] == -1 && $x[1] == 10)

$x[-1, 1] = 20
test_ok($x[-1] == 20 && $x.pop == 20)

# array and/or
test_ok(([1,2,3]&[2,4,6]) == [2])
test_ok(([1,2,3]|[2,4,6]) == [1,2,3,4,6])

# compact
$x = [nil, 1, nil, nil, 5, nil, nil]
$x.compact!
test_ok($x == [1, 5])

# uniq
$x = [1, 1, 4, 2, 5, 4, 5, 1, 2]
$x.uniq!
test_ok($x == [1, 4, 2, 5])

# empty?
test_ok(!$x.empty?)
$x = []
test_ok($x.empty?)

# sort
$x = ["it", "came", "to", "pass", "that", "..."]
$x = $x.sort.join(" ")
test_ok($x == "... came it pass that to")
$x = [2,5,3,1,7]
$x.sort!{|a,b| a<=>b}		# sort with condition
test_ok($x == [1,2,3,5,7])
$x.sort!{|a,b| b-a}		# reverse sort
test_ok($x == [7,5,3,2,1])

# split test
$x = "The Book of Mormon"
test_ok($x.split(//).reverse!.join == $x.reverse)
test_ok($x.reverse == $x.reverse!)
test_ok("1 byte string".split(//).reverse.join(":") == "g:n:i:r:t:s: :e:t:y:b: :1")
$x = "a b c  d"
test_ok($x.split == ['a', 'b', 'c', 'd'])
test_ok($x.split(' ') == ['a', 'b', 'c', 'd'])
test_ok(defined? "a".chomp)
test_ok("abc".scan(/./) == ["a", "b", "c"])
test_ok("1a2b3c".scan(/(\d.)/) == [["1a"], ["2b"], ["3c"]])
# non-greedy match
test_ok("a=12;b=22".scan(/(.*?)=(\d*);?/) == [["a", "12"], ["b", "22"]])

$x = [1]
test_ok(($x * 5).join(":") == '1:1:1:1:1')
test_ok(($x * 1).join(":") == '1')
test_ok(($x * 0).join(":") == '')

*$x = *(1..7).to_a
test_ok($x.size == 7)
test_ok($x == [1, 2, 3, 4, 5, 6, 7])

$x = [1,2,3]
$x[1,0] = $x
test_ok($x == [1,1,2,3,2,3])

$x = [1,2,3]
$x[-1,0] = $x
test_ok($x == [1,2,1,2,3,3])

$x = [1,2,3]
$x.concat($x)
test_ok($x == [1,2,3,1,2,3])

test_check "hash"
$x = {1=>2, 2=>4, 3=>6}

test_ok($x[1] == 2)

test_ok(begin
     for k,v in $x
       raise if k*2 != v
     end
     true
   rescue
     false
   end)

test_ok($x.length == 3)
test_ok($x.has_key?(1))
test_ok($x.has_value?(4))
test_ok($x.values_at(2,3) == [4,6])
test_ok($x == {1=>2, 2=>4, 3=>6})

$z = $x.keys.sort.join(":")
test_ok($z == "1:2:3")

$z = $x.values.sort.join(":")
test_ok($z == "2:4:6")
test_ok($x == $x)

$x.shift
test_ok($x.length == 2)

$z = [1,2]
$x[$z] = 256
test_ok($x[$z] == 256)

$x = Hash.new(0)
$x[1] = 1
test_ok($x[1] == 1)
test_ok($x[2] == 0)

$x = Hash.new([])
test_ok($x[22] == [])
test_ok($x[22].equal?($x[22]))

$x = Hash.new{[]}
test_ok($x[22] == [])
test_ok(!$x[22].equal?($x[22]))

$x = Hash.new{|h,k| $z = k; h[k] = k*2}
$z = 0
test_ok($x[22] == 44)
test_ok($z == 22)
$z = 0
test_ok($x[22] == 44)
test_ok($z == 0)
$x.default = 5
test_ok($x[23] == 5)

$x = Hash.new
def $x.default(k)
  $z = k
  self[k] = k*2
end
$z = 0
test_ok($x[22] == 44)
test_ok($z == 22)
$z = 0
test_ok($x[22] == 44)
test_ok($z == 0)

test_check "iterator"

test_ok(!iterator?)

def ttt
  test_ok(iterator?)
end
ttt{}

# yield at top level
test_ok(!defined?(yield))

$x = [1, 2, 3, 4]
$y = []

# iterator over array
for i in $x
  $y.push i
end
test_ok($x == $y)

# nested iterator
def tt
  1.upto(10) {|i|
    yield i
  }
end

i=0
tt{|i| break if i == 5}
test_ok(i == 0)

def tt2(dummy)
  yield 1
end

def tt3(&block)
  tt2(raise(ArgumentError,""),&block)
end

$x = false
begin
  tt3{}
rescue ArgumentError
  $x = true
rescue Exception
end
test_ok($x)

def tt4 &block
  tt2(raise(ArgumentError,""),&block)
end
$x = false
begin
  tt4{}
rescue ArgumentError
  $x = true
rescue Exception
end
test_ok($x)

# iterator break/redo/next/retry
done = true
loop{
  break
  done = false			# should not reach here
}
test_ok(done)

done = false
$bad = false
loop {
  break if done
  done = true
  next
  $bad = true			# should not reach here
}
test_ok(!$bad)

done = false
$bad = false
loop {
  break if done
  done = true
  redo
  $bad = true			# should not reach here
}
test_ok(!$bad)

$x = []
for i in 1 .. 7
  $x.push i
end
test_ok($x.size == 7)
test_ok($x == [1, 2, 3, 4, 5, 6, 7])

# append method to built-in class
class Array
  def iter_test1
    collect{|e| [e, yield(e)]}.sort{|a,b|a[1]<=>b[1]}
  end
  def iter_test2
    a = collect{|e| [e, yield(e)]}
    a.sort{|a,b|a[1]<=>b[1]}
  end
end
$x = [[1,2],[3,4],[5,6]]
test_ok($x.iter_test1{|x|x} == $x.iter_test2{|x|x})

class IterTest
  def initialize(e); @body = e; end

  def each0(&block); @body.each(&block); end
  def each1(&block); @body.each {|*x| block.call(*x) } end
  def each2(&block); @body.each {|*x| block.call(x) } end
  def each3(&block); @body.each {|x| block.call(*x) } end
  def each4(&block); @body.each {|x| block.call(x) } end
  def each5; @body.each {|*x| yield(*x) } end
  def each6; @body.each {|*x| yield(x) } end
  def each7; @body.each {|x| yield(*x) } end
  def each8; @body.each {|x| yield(x) } end

  def f(a)
    a
  end
end
test_ok(IterTest.new(nil).method(:f).to_proc.call([1]) == [1])
m = /\w+/.match("abc")
test_ok(IterTest.new(nil).method(:f).to_proc.call([m]) == [m])

IterTest.new([0]).each0 {|x| test_ok(x == 0)}
IterTest.new([1]).each1 {|x| test_ok(x == 1)}
IterTest.new([2]).each2 {|x| test_ok(x == [2])}
#IterTest.new([3]).each3 {|x| test_ok(x == 3)}
IterTest.new([4]).each4 {|x| test_ok(x == 4)}
IterTest.new([5]).each5 {|x| test_ok(x == 5)}
IterTest.new([6]).each6 {|x| test_ok(x == [6])}
#IterTest.new([7]).each7 {|x| test_ok(x == 7)}
IterTest.new([8]).each8 {|x| test_ok(x == 8)}

IterTest.new([[0]]).each0 {|x| test_ok(x == [0])}
IterTest.new([[1]]).each1 {|x| test_ok(x == [1])}
IterTest.new([[2]]).each2 {|x| test_ok(x == [[2]])}
IterTest.new([[3]]).each3 {|x| test_ok(x == 3)}
IterTest.new([[4]]).each4 {|x| test_ok(x == [4])}
IterTest.new([[5]]).each5 {|x| test_ok(x == [5])}
IterTest.new([[6]]).each6 {|x| test_ok(x == [[6]])}
IterTest.new([[7]]).each7 {|x| test_ok(x == 7)}
IterTest.new([[8]]).each8 {|x| test_ok(x == [8])}

IterTest.new([[0,0]]).each0 {|*x| test_ok(x == [[0,0]])}
IterTest.new([[8,8]]).each8 {|*x| test_ok(x == [[8,8]])}

def m0(v)
  v
end

def m1
  m0(block_given?)
end
test_ok(m1{p 'test'})
test_ok(!m1)

def m
  m0(block_given?,&Proc.new{})
end
test_ok(m1{p 'test'})
test_ok(!m1)

class C
  include Enumerable
  def initialize
    @a = [1,2,3]
  end
  def each(&block)
    @a.each(&block)
  end
end

test_ok(C.new.collect{|n| n} == [1,2,3])

test_ok(Proc == lambda{}.class)
test_ok(Proc == Proc.new{}.class)
lambda{|a|test_ok(a==1)}.call(1)
def block_test(klass, &block)
  test_ok(klass === block)
end

block_test(NilClass)
block_test(Proc){}

def call_argument_test(state, proc, *args)
  x = state
  begin
    proc.call(*args)
  rescue ArgumentError
    x = !x
  end
  test_ok(x,2)
end

call_argument_test(true, lambda{||})
call_argument_test(false, lambda{||}, 1)
call_argument_test(true, lambda{|a,|}, 1)
call_argument_test(false, lambda{|a,|})
call_argument_test(false, lambda{|a,|}, 1,2)

call_argument_test(true, Proc.new{||})
call_argument_test(true, Proc.new{||}, 1)
call_argument_test(true, Proc.new{|a,|}, 1)
call_argument_test(true, Proc.new{|a,|})
call_argument_test(true, Proc.new{|a,|}, 1,2)

def block_get(&block)
  block
end

test_ok(Proc == block_get{}.class)
call_argument_test(true, block_get{||})
call_argument_test(true, block_get{||}, 1)
call_argument_test(true, block_get{|a,|}, 1)
call_argument_test(true, block_get{|a,|})
call_argument_test(true, block_get{|a,|}, 1,2)

call_argument_test(true, block_get(&lambda{||}))
call_argument_test(false, block_get(&lambda{||}),1)
call_argument_test(true, block_get(&lambda{|a,|}),1)
call_argument_test(false, block_get(&lambda{|a,|}),1,2)

blk = block_get{11}
test_ok(blk.class == Proc)
test_ok(blk.to_proc.class == Proc)
test_ok(blk.clone.call == 11)
test_ok(block_get(&blk).class == Proc)

lmd = lambda{44}
test_ok(lmd.class == Proc)
test_ok(lmd.to_proc.class == Proc)
test_ok(lmd.clone.call == 44)
test_ok(block_get(&lmd).class == Proc)

test_ok(Proc.new{|a,| a}.yield(1,2,3) == 1)
call_argument_test(true, Proc.new{|a,|}, 1,2)

test_ok(Proc.new{|&b| b.call(10)}.call {|x| x} == 10)
test_ok(Proc.new{|a,&b| b.call(a)}.call(12) {|x| x} == 12)

def test_return1
  Proc.new {
    return 55
  }.yield + 5
end
test_ok(test_return1() == 55)
def test_return2
  lambda {
    return 55
  }.call + 5
end
test_ok(test_return2() == 60)

def proc_call(&b)
  b.call
end
def proc_yield()
  yield
end
def proc_return1
  lambda{return 42}.call+1
end
test_ok(proc_return1() == 43)
def proc_return2
  ->{return 42}.call+1
end
test_ok(proc_return2() == 43)
def proc_return3
  proc_call{return 42}+1
end
test_ok(proc_return3() == 42)
def proc_return4
  proc_yield{return 42}+1
end
test_ok(proc_return4() == 42)

def ljump_test(state, proc, *args)
  x = state
  begin
    proc.call(*args)
  rescue LocalJumpError
    x = !x
  end
  test_ok(x,2)
end

ljump_test(false, block_get{break})
ljump_test(true, lambda{break})

def exit_value_test(&block)
  block.call
rescue LocalJumpError
  $!.exit_value
end

test_ok(45 == exit_value_test{break 45})

test_ok(55 == begin
              block_get{break 55}.call
            rescue LocalJumpError
              $!.exit_value
            end)

def block_call(&block)
  block.call
end

def test_b1
  block_call{break 11}
end
test_ok(test_b1() == 11)

def ljump_rescue(r)
  begin
    yield
  rescue LocalJumpError => e
    r if /from proc-closure/ =~ e.message
  end
end

def test_b2
  ljump_rescue(22) do
    block_get{break 21}.call
  end
end
test_ok(test_b2() == 22)

def test_b3
  ljump_rescue(33) do
    Proc.new{break 31}.yield
  end
end
test_ok(test_b3() == 33)

def test_b4
  lambda{break 44}.call
end
test_ok(test_b4() == 44)

def test_b5
  ljump_rescue(55) do
    b = block_get{break 54}
    block_call(&b)
  end
end
test_ok(test_b5() == 55)

def test_b6
  b = lambda{break 67}
  block_call(&b)
  66
end
test_ok(test_b6() == 66)

def util_r7
  block_get{break 78}
end

def test_b7
  b = util_r7()
  ljump_rescue(77) do
    block_call(&b)
  end
end
test_ok(test_b7() == 77)

def util_b8(&block)
  block_call(&block)
end

def test_b8
  util_b8{break 88}
end
test_ok(test_b8() == 88)

def util_b9(&block)
  lambda{block.call; 98}.call
end

def test_b9
  util_b9{break 99}
end
test_ok(test_b9() == 99)

def util_b10
  util_b9{break 100}
end

def test_b10
  util_b10()
end
test_ok(test_b10() == 100)

def test_b11
  ljump_rescue(111) do
    loop do
      Proc.new{break 110}.yield
      break 112
    end
  end
end
test_ok(test_b11() == 111)

def test_b12
  loop do
    break lambda{break 122}.call
    break 121
  end
end
test_ok(test_b12() == 122)

def test_b13
  ljump_rescue(133) do
    while true
      Proc.new{break 130}.yield
      break 131
    end
  end
end
test_ok(test_b13() == 133)

def test_b14
  while true
    break lambda{break 144}.call
    break 143
  end
end
test_ok(test_b14() == 144)

def test_b15
  [0].each {|c| yield 1 }
  156
end
test_ok(test_b15{|e| break 155 } == 155)

def marity_test(m)
  method = method(m)
  test_ok(method.arity == method.to_proc.arity, 2)
end
marity_test(:test_ok)
marity_test(:marity_test)
marity_test(:p)

lambda(&method(:test_ok)).call(true)
lambda(&block_get{|a,n| test_ok(a,n)}).call(true, 2)

class ITER_TEST1
   def a
     block_given?
   end
end

class ITER_TEST2 < ITER_TEST1
   def a
     test_ok(super)
     super
   end
end
test_ok(ITER_TEST2.new.a {})

class ITER_TEST3
  def foo x
    return yield if block_given?
    x
  end
end

class ITER_TEST4 < ITER_TEST3
  def foo x
    test_ok(super == yield)
    test_ok(super(x, &nil) == x)
  end
end

ITER_TEST4.new.foo(44){55}

class ITER_TEST5
   def tt(aa)
     aa
   end

   def uu(a)
      class << self
         define_method(:tt) do |sym|
            super(sym)
         end
      end
   end

   def xx(*x)
     x.size
   end
end

a = ITER_TEST5.new
a.uu(12)
test_ok(a.tt(1) == 1)

class ITER_TEST6 < ITER_TEST5
   def xx(*a)
      a << 12
      super
   end
end

test_ok(ITER_TEST6.new.xx([24]) == 2)

test_check "float"
test_ok(2.6.floor == 2)
test_ok((-2.6).floor == -3)
test_ok(2.6.ceil == 3)
test_ok((-2.6).ceil == -2)
test_ok(2.6.truncate == 2)
test_ok((-2.6).truncate == -2)
test_ok(2.6.round == 3)
test_ok((-2.4).truncate == -2)
test_ok((13.4 % 1 - 0.4).abs < 0.0001)
nan = 0.0/0
def nan_test(x,y)
  test_ok(x != y)
  test_ok((x < y) == false)
  test_ok((x > y) == false)
  test_ok((x <= y) == false)
  test_ok((x >= y) == false)
end
nan_test(nan, nan)
nan_test(nan, 0)
nan_test(nan, 1)
nan_test(nan, -1)
nan_test(nan, 1000)
nan_test(nan, -1000)
nan_test(nan, 1_000_000_000_000)
nan_test(nan, -1_000_000_000_000)
nan_test(nan, 100.0);
nan_test(nan, -100.0);
nan_test(nan, 0.001);
nan_test(nan, -0.001);
nan_test(nan, 1.0/0);
nan_test(nan, -1.0/0);

#s = "3.7517675036461267e+17"
#test_ok(s == sprintf("%.16e", s.to_f))
f = 3.7517675036461267e+17
test_ok(f == sprintf("%.16e", f).to_f)


test_check "bignum"
def fact(n)
  return 1 if n == 0
  f = 1
  while n>0
    f *= n
    n -= 1
  end
  return f
end
$x = fact(40)
test_ok($x == $x)
test_ok($x == fact(40))
test_ok($x < $x+2)
test_ok($x > $x-2)
test_ok($x == 815915283247897734345611269596115894272000000000)
test_ok($x != 815915283247897734345611269596115894272000000001)
test_ok($x+1 == 815915283247897734345611269596115894272000000001)
test_ok($x/fact(20) == 335367096786357081410764800000)
$x = -$x
test_ok($x == -815915283247897734345611269596115894272000000000)
test_ok(2-(2**32) == -(2**32-2))
test_ok(2**32 - 5 == (2**32-3)-2)

$good = true;
for i in 1000..1014
  $good = false if ((1 << i) != (2**i))
end
test_ok($good)

$good = true;
n1= 1 << 1000
for i in 1000..1014
  $good = false if ((1 << i) != n1)
  n1 *= 2
end
test_ok($good)

$good = true;
n2=n1
for i in 1..10
  n1 = n1 / 2
  n2 = n2 >> 1
  $good = false if (n1 != n2)
end
test_ok($good)

$good = true;
for i in 4000..4096
  n1 = 1 << i;
  if (n1**2-1) / (n1+1) != (n1-1)
    $good = false
  end
end
test_ok($good)

b = 10**80
a = b * 9 + 7
test_ok(7 == a.modulo(b))
test_ok(-b + 7 == a.modulo(-b))
test_ok(b + -7 == (-a).modulo(b))
test_ok(-7 == (-a).modulo(-b))
test_ok(7 == a.remainder(b))
test_ok(7 == a.remainder(-b))
test_ok(-7 == (-a).remainder(b))
test_ok(-7 == (-a).remainder(-b))

test_ok(10**40+10**20 == 10000000000000000000100000000000000000000)
test_ok(10**40/10**20 == 100000000000000000000)

a = 677330545177305025495135714080
b = 14269972710765292560
test_ok(a % b == 0)
test_ok(-a % b == 0)

def shift_test(a)
  b = a / (2 ** 32)
  c = a >> 32
  test_ok(b == c)

  b = a * (2 ** 32)
  c = a << 32
  test_ok(b == c)
end

shift_test(-4518325415524767873)
shift_test(-0xfffffffffffffffff)

test_check "string & char"

test_ok("abcd" == "abcd")
test_ok("abcd" =~ /abcd/)
test_ok("abcd" === "abcd")
# compile time string concatenation
test_ok("ab" "cd" == "abcd")
test_ok("#{22}aa" "cd#{44}" == "22aacd44")
test_ok("#{22}aa" "cd#{44}" "55" "#{66}" == "22aacd445566")
test_ok("abc" !~ /^$/)
test_ok("abc\n" !~ /^$/)
test_ok("abc" !~ /^d*$/)
test_ok(("abc" =~ /d*$/) == 3)
test_ok("" =~ /^$/)
test_ok("\n" =~ /^$/)
test_ok("a\n\n" =~ /^$/)
test_ok("abcabc" =~ /.*a/ && $& == "abca")
test_ok("abcabc" =~ /.*c/ && $& == "abcabc")
test_ok("abcabc" =~ /.*?a/ && $& == "a")
test_ok("abcabc" =~ /.*?c/ && $& == "abc")
test_ok(/(.|\n)*?\n(b|\n)/ =~ "a\nb\n\n" && $& == "a\nb")

test_ok(/^(ab+)+b/ =~ "ababb" && $& == "ababb")
test_ok(/^(?:ab+)+b/ =~ "ababb" && $& == "ababb")
test_ok(/^(ab+)+/ =~ "ababb" && $& == "ababb")
test_ok(/^(?:ab+)+/ =~ "ababb" && $& == "ababb")

test_ok(/(\s+\d+){2}/ =~ " 1 2" && $& == " 1 2")
test_ok(/(?:\s+\d+){2}/ =~ " 1 2" && $& == " 1 2")

$x = <<END;
ABCD
ABCD
END
$x.gsub!(/((.|\n)*?)B((.|\n)*?)D/, '\1\3')
test_ok($x == "AC\nAC\n")

test_ok("foobar" =~ /foo(?=(bar)|(baz))/)
test_ok("foobaz" =~ /foo(?=(bar)|(baz))/)

$foo = "abc"
test_ok("#$foo = abc" == "abc = abc")
test_ok("#{$foo} = abc" == "abc = abc")

foo = "abc"
test_ok("#{foo} = abc" == "abc = abc")

test_ok('-' * 5 == '-----')
test_ok('-' * 1 == '-')
test_ok('-' * 0 == '')

foo = '-'
test_ok(foo * 5 == '-----')
test_ok(foo * 1 == '-')
test_ok(foo * 0 == '')

$x = "a.gif"
test_ok($x.sub(/.*\.([^\.]+)$/, '\1') == "gif")
test_ok($x.sub(/.*\.([^\.]+)$/, 'b.\1') == "b.gif")
test_ok($x.sub(/.*\.([^\.]+)$/, '\2') == "")
test_ok($x.sub(/.*\.([^\.]+)$/, 'a\2b') == "ab")
test_ok($x.sub(/.*\.([^\.]+)$/, '<\&>') == "<a.gif>")

# character constants(assumes ASCII)
test_ok("a"[0] == ?a)
test_ok(?a == ?a)
test_ok(?\C-a == "\1")
test_ok(?\M-a == "\341")
test_ok(?\M-\C-a == "\201")
test_ok("a".upcase![0] == ?A)
test_ok("A".downcase![0] == ?a)
test_ok("abc".tr!("a-z", "A-Z") == "ABC")
test_ok("aabbcccc".tr_s!("a-z", "A-Z") == "ABC")
test_ok("abcc".squeeze!("a-z") == "abc")
test_ok("abcd".delete!("bc") == "ad")

$x = "abcdef"
$y = [ ?a, ?b, ?c, ?d, ?e, ?f ]
$bad = false
$x.each_byte {|i|
  if i.chr != $y.shift
    $bad = true
    break
  end
}
test_ok(!$bad)

s = "a string"
s[0..s.size]="another string"
test_ok(s == "another string")

s = <<EOS
#{
[1,2,3].join(",")
}
EOS
test_ok(s == "1,2,3\n")
test_ok("Just".to_i(36) == 926381)
test_ok("-another".to_i(36) == -23200231779)
test_ok(1299022.to_s(36) == "ruby")
test_ok(-1045307475.to_s(36) == "-hacker")
test_ok("Just_another_Ruby_hacker".to_i(36) == 265419172580680477752431643787347)
test_ok(-265419172580680477752431643787347.to_s(36) == "-justanotherrubyhacker")

a = []
(0..255).each {|n|
  ch = [n].pack("C")
  a.push ch if /a#{Regexp.quote ch}b/x =~ "ab"
}
test_ok(a.size == 0)

test_check "assignment"
a = nil
test_ok(defined?(a))
test_ok(a == nil)

# multiple asignment
a, b = 1, 2
test_ok(a == 1 && b == 2)

a, b = b, a
test_ok(a == 2 && b == 1)

a, = 1,2
test_ok(a == 1)

a, *b = 1, 2, 3
test_ok(a == 1 && b == [2, 3])

a, (b, c), d = 1, [2, 3], 4
test_ok(a == 1 && b == 2 && c == 3 && d == 4)

*a = 1, 2, 3
test_ok(a == [1, 2, 3])

*a = 4
test_ok(a == [4])

*a = nil
test_ok(a == [nil])

test_check "call"
def aaa(a, b=100, *rest)
  res = [a, b]
  res += rest if rest
  return res
end

# not enough argument
begin
  aaa()				# need at least 1 arg
  test_ok(false)
rescue
  test_ok(true)
end

begin
  aaa				# no arg given (exception raised)
  test_ok(false)
rescue
  test_ok(true)
end

test_ok(aaa(1) == [1, 100])
test_ok(aaa(1, 2) == [1, 2])
test_ok(aaa(1, 2, 3, 4) == [1, 2, 3, 4])
test_ok(aaa(1, *[2, 3, 4]) == [1, 2, 3, 4])

test_check "proc"
$proc = Proc.new{|i| i}
test_ok($proc.call(2) == 2)
test_ok($proc.call(3) == 3)

$proc = Proc.new{|i| i*2}
test_ok($proc.call(2) == 4)
test_ok($proc.call(3) == 6)

Proc.new{
  iii=5				# nested local variable
  $proc = Proc.new{|i|
    iii = i
  }
  $proc2 = Proc.new {
    $x = iii			# nested variables shared by procs
  }
  # scope of nested variables
  test_ok(defined?(iii))
}.call
test_ok(!defined?(iii))		# out of scope

loop{iii=5; test_ok(eval("defined? iii")); break}
loop {
  iii = 10
  def dyna_var_check
    loop {
      test_ok(!defined?(iii))
      break
    }
  end
  dyna_var_check
  break
}
$x=0
$proc.call(5)
$proc2.call
test_ok($x == 5)

if defined? Process.kill
  test_check "signal"

  $x = 0
  trap "SIGINT", Proc.new{|sig| $x = 2}
  Process.kill "SIGINT", $$
  100.times {
    sleep 0.1
    break if $x != 0
  }
  test_ok($x == 2)

  trap "SIGINT", Proc.new{raise "Interrupt"}

  x = false
  begin
    Process.kill "SIGINT", $$
    sleep 0.1
  rescue
    x = $!
  end
  test_ok(x && /Interrupt/ =~ x.message)
end

test_check "eval"
test_ok(eval("") == nil)
$bad=false
eval 'while false; $bad = true; print "foo\n" end'
test_ok(!$bad)

test_ok(eval('TRUE'))
test_ok(eval('true'))
test_ok(!eval('NIL'))
test_ok(!eval('nil'))
test_ok(!eval('FALSE'))
test_ok(!eval('false'))

$foo = 'test_ok(true)'
begin
  eval $foo
rescue
  test_ok(false)
end

test_ok(eval("$foo") == 'test_ok(true)')
test_ok(eval("true") == true)
i = 5
test_ok(eval("i == 5"))
test_ok(eval("i") == 5)
test_ok(eval("defined? i"))

# eval with binding
def test_ev
  local1 = "local1"
  lambda {
    local2 = "local2"
    return binding
  }.call
end

$x = test_ev
test_ok(eval("local1", $x) == "local1") # normal local var
test_ok(eval("local2", $x) == "local2") # nested local var
$bad = true
begin
  p eval("local1")
rescue NameError		# must raise error
  $bad = false
end
test_ok(!$bad)

module EvTest
  EVTEST1 = 25
  evtest2 = 125
  $x = binding
end
test_ok(eval("EVTEST1", $x) == 25)	# constant in module
test_ok(eval("evtest2", $x) == 125)	# local var in module
$bad = true
begin
  eval("EVTEST1")
rescue NameError		# must raise error
  $bad = false
end
test_ok(!$bad)

x = binding #! YARV Limitation: Proc.new{}
eval "i4 = 1", x
test_ok(eval("i4", x) == 1)
x = Proc.new{binding}.call #! YARV Limitation: Proc.new{Proc.new{}}.call
eval "i4 = 22", x
test_ok(eval("i4", x) == 22)
$x = []
x = Proc.new{binding}.call #! YARV Limitation: Proc.new{Proc.new{}}.call
eval "(0..9).each{|i5| $x[i5] = Proc.new{i5*2}}", x
test_ok($x[4].call == 8)

x = binding
eval "i = 1", x
test_ok(eval("i", x) == 1)
x = Proc.new{binding}.call
eval "i = 22", x
test_ok(eval("i", x) == 22)
$x = []
x = Proc.new{binding}.call
eval "(0..9).each{|i5| $x[i5] = Proc.new{i5*2}}", x
test_ok($x[4].call == 8)
x = Proc.new{binding}.call
eval "for i6 in 1..1; j6=i6; end", x
test_ok(eval("defined? i6", x))
test_ok(eval("defined? j6", x))

Proc.new {
  p = binding
  eval "foo11 = 1", p
  foo22 = 5
  Proc.new{foo11=22}.call
  Proc.new{foo22=55}.call
  test_ok(eval("foo11", p) == eval("foo11"))
  test_ok(eval("foo11") == 1)
  test_ok(eval("foo22", p) == eval("foo22"))
  test_ok(eval("foo22") == 55)
}.call if false #! YARV Limitation

#! YARV Limitation: p1 = Proc.new{i7 = 0; Proc.new{i7}}.call
p1 = Proc.new{i7 = 0; binding}.call
#! YARV Limitation: test_ok(p1.call == 0)
eval "i7=5", p1
#! YARV Limitation: test_ok(p1.call == 5)
test_ok(!defined?(i7))

if false #! YARV Limitation
p1 = Proc.new{i7 = 0; Proc.new{i7}}.call
i7 = nil
test_ok(p1.call == 0)
eval "i7=1", p1
test_ok(p1.call == 1)
eval "i7=5", p1
test_ok(p1.call == 5)
test_ok(i7 == nil)
end

test_check "system"
test_ok(`echo foobar` == "foobar\n")
test_ok(`./miniruby -e 'print "foobar"'` == 'foobar')

tmp = open("script_tmp", "w")
tmp.print "print $zzz\n";
tmp.close

test_ok(`./miniruby -s script_tmp -zzz` == 'true')
test_ok(`./miniruby -s script_tmp -zzz=555` == '555')

tmp = open("script_tmp", "w")
tmp.print "#! /usr/local/bin/ruby -s\n";
tmp.print "print $zzz\n";
tmp.close

test_ok(`./miniruby script_tmp -zzz=678` == '678')

tmp = open("script_tmp", "w")
tmp.print "this is a leading junk\n";
tmp.print "#! /usr/local/bin/ruby -s\n";
tmp.print "print $zzz\n";
tmp.print "__END__\n";
tmp.print "this is a trailing junk\n";
tmp.close

test_ok(`./miniruby -x script_tmp` == '')
test_ok(`./miniruby -x script_tmp -zzz=555` == '555')

tmp = open("script_tmp", "w")
for i in 1..5
  tmp.print i, "\n"
end
tmp.close

`./miniruby -i.bak -pe '$_.sub!(/^[0-9]+$/){$&.to_i * 5}' script_tmp`
done = true
tmp = open("script_tmp", "r")
while tmp.gets
  if $_.to_i % 5 != 0
    done = false
    break
  end
end
tmp.close
test_ok(done)

File.unlink "script_tmp" or `/bin/rm -f "script_tmp"`
File.unlink "script_tmp.bak" or `/bin/rm -f "script_tmp.bak"`

test_check "const"
TEST1 = 1
TEST2 = 2

module Const
  TEST3 = 3
  TEST4 = 4
end

module Const2
  TEST3 = 6
  TEST4 = 8
end

include Const

test_ok([TEST1,TEST2,TEST3,TEST4] == [1,2,3,4])

include Const2
STDERR.print "intentionally redefines TEST3, TEST4\n" if $VERBOSE
test_ok([TEST1,TEST2,TEST3,TEST4] == [1,2,6,8])


test_ok((String <=> Object) == -1)
test_ok((Object <=> String) == 1)
test_ok((Array <=> String) == nil)

test_check "clone"
foo = Object.new
def foo.test
  "test"
end
bar = foo.clone
def bar.test2
  "test2"
end

test_ok(bar.test2 == "test2")
test_ok(bar.test == "test")
test_ok(foo.test == "test")

begin
  foo.test2
  test_ok false
rescue NoMethodError
  test_ok true
end

module M001; end
module M002; end
module M003; include M002; end
module M002; include M001; end
module M003; include M002; end

test_ok(M003.ancestors == [M003, M002, M001])

test_check "marshal"
$x = [1,2,3,[4,5,"foo"],{1=>"bar"},2.5,fact(30)]
$y = Marshal.dump($x)
test_ok($x == Marshal.load($y))

StrClone=String.clone;
test_ok(Marshal.load(Marshal.dump(StrClone.new("abc"))).class == StrClone)

[[1,2,3,4], [81, 2, 118, 3146]].each { |w,x,y,z|
  a = (x.to_f + y.to_f / z.to_f) * Math.exp(w.to_f / (x.to_f + y.to_f / z.to_f))
  ma = Marshal.dump(a)
  b = Marshal.load(ma)
  test_ok(a == b)
}

test_check "pack"

$format = "c2x5CCxsdils_l_a6";
# Need the expression in here to force ary[5] to be numeric.  This avoids
# test2 failing because ary2 goes str->numeric->str and ary does not.
ary = [1,-100,127,128,32767,987.654321098 / 100.0,12345,123456,-32767,-123456,"abcdef"]
$x = ary.pack($format)
ary2 = $x.unpack($format)

test_ok(ary.length == ary2.length)
test_ok(ary.join(':') == ary2.join(':'))
test_ok($x =~ /def/)

$x = [-1073741825]
test_ok($x.pack("q").unpack("q") == $x)

test_check "math"
test_ok(Math.sqrt(4) == 2)

include Math
test_ok(sqrt(4) == 2)

test_check "struct"
struct_test = Struct.new("Test", :foo, :bar)
test_ok(struct_test == Struct::Test)

test = struct_test.new(1, 2)
test_ok(test.foo == 1 && test.bar == 2)
test_ok(test[0] == 1 && test[1] == 2)

a, b = test.to_a
test_ok(a == 1 && b == 2)

test[0] = 22
test_ok(test.foo == 22)

test.bar = 47
test_ok(test.bar == 47)

test_check "variable"
test_ok($$.instance_of?(Fixnum))

# read-only variable
begin
  $$ = 5
  test_ok false
rescue NameError
  test_ok true
end

foobar = "foobar"
$_ = foobar
test_ok($_ == foobar)

class Gods
  @@rule = "Uranus"		# private to Gods
  def ruler0
    @@rule
  end

  def self.ruler1		# <= per method definition style
    @@rule
  end
  class << self			# <= multiple method definition style
    def ruler2
      @@rule
    end
  end
end

module Olympians
  @@rule ="Zeus"
  def ruler3
    @@rule
  end
end

class Titans < Gods
  @@rule = "Cronus"		# do not affect @@rule in Gods
  include Olympians
  def ruler4
    @@rule
  end
end

test_ok(Gods.new.ruler0 == "Cronus")
test_ok(Gods.ruler1 == "Cronus")
test_ok(Gods.ruler2 == "Cronus")
test_ok(Titans.ruler1 == "Cronus")
test_ok(Titans.ruler2 == "Cronus")
atlas = Titans.new
test_ok(atlas.ruler0 == "Cronus")
test_ok(atlas.ruler3 == "Zeus")
test_ok(atlas.ruler4 == "Cronus")

test_check "trace"
$x = 1234
$y = 0
trace_var :$x, Proc.new{$y = $x}
$x = 40414
test_ok($y == $x)

untrace_var :$x
$x = 19660208
test_ok($y != $x)

trace_var :$x, Proc.new{$x *= 2}
$x = 5
test_ok($x == 10)

untrace_var :$x

test_check "defined?"

test_ok(defined?($x))		# global variable
test_ok(defined?($x) == 'global-variable')# returns description

foo=5
test_ok(defined?(foo))		# local variable

test_ok(defined?(Array))	# constant
test_ok(defined?(Object.new))	# method
test_ok(!defined?(Object.print))# private method
test_ok(defined?(1 == 2))	# operator expression

class Foo
  def foo
    p :foo
  end
  protected :foo
  def bar(f)
    test_ok(defined?(self.foo))
    test_ok(defined?(f.foo))
  end
end
f = Foo.new
test_ok(defined?(f.foo) == nil)
f.bar(f)

def defined_test
  return !defined?(yield)
end

test_ok(defined_test)		# not iterator
test_ok(!defined_test{})	# called as iterator

test_check "alias"
class Alias0
  def foo; "foo" end
end
class Alias1<Alias0
  alias bar foo
  def foo; "foo+" + super end
end
class Alias2<Alias1
  alias baz foo
  undef foo
end

x = Alias2.new
test_ok(x.bar == "foo")
test_ok(x.baz == "foo+foo")

# test_check for cache
test_ok(x.baz == "foo+foo")

class Alias3<Alias2
  def foo
    defined? super
  end
  def bar
    defined? super
  end
  def quux
    defined? super
  end
end
x = Alias3.new
test_ok(!x.foo)
test_ok(x.bar)
test_ok(!x.quux)

test_check "path"
test_ok(File.basename("a") == "a")
test_ok(File.basename("a/b") == "b")
test_ok(File.basename("a/b/") == "b")
test_ok(File.basename("/") == "/")
test_ok(File.basename("//") == "/")
test_ok(File.basename("///") == "/")
test_ok(File.basename("a/b////") == "b")
test_ok(File.basename("a.rb", ".rb") == "a")
test_ok(File.basename("a.rb///", ".rb") == "a")
test_ok(File.basename("a.rb///", ".*") == "a")
test_ok(File.basename("a.rb///", ".c") == "a.rb")
test_ok(File.dirname("a") == ".")
test_ok(File.dirname("/") == "/")
test_ok(File.dirname("/a") == "/")
test_ok(File.dirname("a/b") == "a")
test_ok(File.dirname("a/b/c") == "a/b")
test_ok(File.dirname("/a/b/c") == "/a/b")
test_ok(File.dirname("/a/b/") == "/a")
test_ok(File.dirname("/a/b///") == "/a")
case Dir.pwd
when %r'\A\w:'
  test_ok(/\A\w:\/\z/ =~ File.expand_path(".", "/"))
  test_ok(/\A\w:\/a\z/ =~ File.expand_path("a", "/"))
  dosish = true
when %r'\A//'
  test_ok(%r'\A//[^/]+/[^/]+\z' =~ File.expand_path(".", "/"))
  test_ok(%r'\A//[^/]+/[^/]+/a\z' =~ File.expand_path(".", "/"))
  dosish = true
else
  test_ok(File.expand_path(".", "/") == "/")
  test_ok(File.expand_path("sub", "/") == "/sub")
end
if dosish
  test_ok(File.expand_path("/", "//machine/share/sub") == "//machine/share")
  test_ok(File.expand_path("/dir", "//machine/share/sub") == "//machine/share/dir")
  test_ok(File.expand_path("/", "z:/sub") == "z:/")
  test_ok(File.expand_path("/dir", "z:/sub") == "z:/dir")
end
test_ok(File.expand_path(".", "//") == "//")
test_ok(File.expand_path("sub", "//") == "//sub")

# test_check "Proc#binding"
ObjectSpace.each_object(Proc){|o|
  begin
    b = o.binding
    eval 'self', b
  rescue ArgumentError
  end
}

test_check "gc"
begin
  1.upto(10000) {
    tmp = [0,1,2,3,4,5,6,7,8,9]
  }
  tmp = nil
  test_ok true
rescue
  test_ok false
end
class S
  def initialize(a)
    @a = a
  end
end
l=nil
100000.times {
  l = S.new(l)
}
GC.start
test_ok true   # reach here or dumps core
l = []
100000.times {
  l.push([l])
}
GC.start
test_ok true   # reach here or dumps core

ObjectSpace.each_object{|o|
  o.class.name
}

test_ok true   # reach here or dumps core

if $failed > 0
  printf "not ok/test: %d failed %d\n", $ntest, $failed
else
  printf "end of test(test: %d)\n", $ntest
end
