assert_equal %q{ok}, %q{
  def m
    :ng1
    mm{
      yield
    }
    :ng2
  end

  def mm
    :ng3
    yield
    :ng4
  end

  m{
    break :ok
  }
}
assert_equal %q{ok}, %q{
  3.times{
    break :ok
  }
}
assert_equal %q{}, %q{
  catch(:foo){
    throw :foo
  }
}
assert_equal %q{false}, %q{
  catch(:foo){
    throw :foo, false
  }
}
assert_equal %q{}, %q{
  catch(:foo){
    throw :foo, nil
  }
}
assert_equal %q{ok}, %q{
  catch(:foo){
    throw :foo, :ok
  }
}
assert_equal %q{}, %q{
  catch(:foo){
    1.times{
      throw :foo
    }
  }
}
assert_equal %q{ok}, %q{
  catch(:foo){
    1.times{
      throw :foo, :ok
    }
  }
}
assert_equal %q{ok}, %q{
  catch(:foo){
    catch(:bar){
      throw :foo, :ok
    }
    :ng
  }
}
assert_equal %q{ok}, %q{
  catch(:foo){
    catch(:bar){
      1.times{
        throw :foo, :ok
      }
    }
    :ng
  }
}
assert_equal %q{2}, %q{
  module Enumerable
    def all_?
      self.each{|e|
        unless yield(e)
          return false
        end
      }
      true
    end
  end

  xxx = 0
  [1,2].each{|bi|
    [3,4].each{|bj|
      [true, nil, true].all_?{|be| be}
      break
    }
    xxx += 1
  }
  xxx
}
assert_equal %q{ok}, %q{
  def m
    yield
  end

  m{
    begin
    ensure
      break :ok
    end
  }
}
assert_equal %q{ok}, %q{
  def m
    yield
    :ok
  end
  i=0
  m{
    if i>10
      i*i
    else
      i+=1
      next
    end
  }
}
assert_equal %q{ok}, %q{
  def m
    yield
  end

  m{
    next :ok
  }
}
assert_equal %q{131}, %q{
  def m
    yield + 10
  end
  i=0
  m{
    if i>10
      i*i
    else
      i+=1
      redo
    end
  }
}
assert_equal %q{ok}, %q{
begin
  eval %q{
    1.times{
      retry
    }
  }
rescue SyntaxError
  :ok
end
}
assert_equal %q{3}, %q{
  def m
    return 3
  end
  m
}
assert_equal %q{ok}, %q{
  def m
    :ng1
    mm{
      return :ok
    }
    :ng2
  end

  def mm
    :ng3
    yield
    :ng4
  end
  m
}
assert_equal %q{100}, %q{
  $i = 0
  def m
    begin
      iter{
        return
      }
    ensure
      $i = 100
    end
  end

  def iter
    yield
  end
  m
  $i
}
assert_equal %q{ok}, %q{
  def m
    begin
      raise
    rescue
      return :ok
    end
    :ng
  end
  m
}
assert_equal %q{1}, %q{
  def m
    begin
      raise
    rescue
      return 1
    end
  end

  m
}
assert_equal %q{1}, %q{
  def m
    begin
      #
    ensure
      return 1
    end
  end

  m
}
assert_equal 'ok', %q{
  begin
    catch {|t| throw t, :ok }
  rescue ArgumentError
    :ng
  end
}, '[ruby-dev:31609]'

assert_equal "1", %q{
  catch do |t|
    begin
      throw t, 1
      2
    ensure
      3
    end
  end
}, "[ruby-dev:31698]"

assert_normal_exit %q{
  f = 0
  1.times do
    begin
      f += 1
    ensure
      redo unless f > 2
    end
  end
}

assert_normal_exit %q{
  -> do
    1.times do
      begin
        raise
      rescue
        return
      end
    end
  end.call
}

assert_normal_exit %q{
  while true
    begin
      raise
      next
    rescue
    end
    break
  end
}, '[ruby-core:28172]'

assert_equal "true", %q{
  class Object
    def return_eigenclass
      class << self
        return self
      end
    end
  end
  s = "foo"
  s.return_eigenclass == class << s; self; end
}, '[ruby-core:21379]'

assert_equal "true", %q{
  class Object
    def yield_eigenclass
      class << self
        yield self
      end
    end
  end
  s = "foo"
  s.yield_eigenclass {|c| c == class << s; self; end }
}, '[ruby-dev:40975]'
