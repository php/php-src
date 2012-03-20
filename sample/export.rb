# method access permission
# output:
#	foobar
#	Foo

class Foo
  public :printf
  def baz
    print "baz\n"
  end
  private :baz

  def quux
    print "in QUUX "
    baz()
  end
end

def foobar
  print "foobar\n"
end

f = Foo.new
#Foo.private :printf
class Foo			# redefines foobar's scope
  public :foobar
end
f.foobar
f.printf "%s\n", Foo

f.quux

class Bar<Foo
  def quux
    super
    baz()
  end
end

Bar.new.quux
