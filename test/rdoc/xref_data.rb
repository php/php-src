XREF_DATA = <<-XREF_DATA
class C1

  attr :attr

  # :section: separate

  attr_reader :attr_reader
  attr_writer :attr_writer

  # :section:
  attr_accessor :attr_accessor

  CONST = :const

  def self.m
  end

  def m foo
  end

end

class C2
  def b
  end

  alias a b

  class C3
    def m
    end

    class H1
      def m?
      end
    end
  end
end

class C3
  class H1
  end

  class H2 < H1
  end
end

class C4
  class C4
  end
end

class C5
  class C1
  end
end

module M1
  def m
  end
end

module M1::M2
end

class Parent
  def m() end
  def self.m() end
end

class Child < Parent
end

XREF_DATA

