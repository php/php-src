class Object
  @@golf_hash = {}
  def method_missing m, *a, &b
    t = @@golf_hash[ [m,self.class] ] ||= matching_methods(m)[0]
    if t && b
      __send__(t, *a) {|*args|
        b.binding.eval("proc{|golf_matchdata| $~ = golf_matchdata }").call($~) if $~
        b.call(*args)
      }
    else
      t ? __send__(t, *a, &b) : super
    end
  end

  def matching_methods(s='', m=callable_methods)
    r=/^#{s.to_s.gsub(/./){"(.*?)"+Regexp.escape($&)}}/
    m.grep(r).sort_by do |i|
      i.to_s.match(r).captures.map(&:size) << i
    end
  end

  def self.const_missing c
    t = @@golf_hash[ [c,self.class] ] ||= matching_methods(c,constants)[0]
    t and return const_get(t)
    raise NameError, "uninitialized constant #{c}", caller(1)
  end

  def shortest_abbreviation(s='', m=callable_methods)
    s=s.to_s
    our_case = (?A..?Z)===s[0]
    if m.index(s.to_sym)
      1.upto(s.size){|z|s.scan(/./).combination(z).map{|trial|
        next unless ((?A..?Z)===trial[0]) == our_case
        trial*=''
        return trial if matching_methods(trial,m)[0].to_s==s
      }}
    else
      nil
    end
  end

  def callable_methods
    self.class == Object ? methods + private_methods : methods
  end

  private

  def h(a='H', b='w', c='!')
    puts "#{a}ello, #{b}orld#{c}"
  end

  alias say puts

  def do_while
    0 while yield
  end

  def do_until
    0 until yield
  end
end

class Array
  alias old_to_s to_s
  alias to_s join
end

class FalseClass
  alias old_to_s to_s
  def to_s
    ""
  end
end

class Integer
  alias each times
  include Enumerable
end

class String
  alias / split

  def to_a
    split('')
  end

  (Array.instance_methods-instance_methods-[:to_ary,:transpose,:flatten,:flatten!,:compact,:compact!,:assoc,:rassoc]).each{|meth|
    eval"
    def #{meth}(*args, &block)
      a=to_a
      result = a.#{meth}(*args, &block)
      replace(a.join)
      if result.class == Array
        Integer===result[0] ? result.pack('c*') : result.join
      elsif result.class == Enumerator
        result.map(&:join).to_enum
      else
        result
      end
    end"
  }
end

class Enumerator
  alias old_to_s to_s
  (Array.instance_methods-instance_methods-[:replace]+[:to_s]).each{|meth|
    eval"
    def #{meth}(*args, &block)
      to_a.#{meth}(*args, &block)
    end"
  }
  alias old_inspect inspect
  alias inspect old_to_s
end

class Symbol
  def call(*args, &block)
    proc do |recv|
      recv.__send__(self, *args, &block)
    end
  end
end
