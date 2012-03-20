assert_equal 'A', %q{
  class A
    @@a = 'A'
    def a=(x)
      @@a = x
    end
    def a
      @@a
    end
  end

  B = A.dup
  B.new.a = 'B'
  A.new.a
}, '[ruby-core:17019]'

assert_equal 'ok', %q{
  def m
    lambda{
      proc{
        return :ng1
      }
    }.call.call
    :ng2
  end

  begin
    m()
  rescue LocalJumpError
    :ok
  end
}

assert_normal_exit %q{
  r = Range.allocate
  def r.<=>(o) true end
  r.instance_eval { initialize r, r }
  r.inspect
}
