module Rake

  ####################################################################
  # InvocationChain tracks the chain of task invocations to detect
  # circular dependencies.
  class InvocationChain
    def initialize(value, tail)
      @value = value
      @tail = tail
    end

    def member?(obj)
      @value == obj || @tail.member?(obj)
    end

    def append(value)
      if member?(value)
        fail RuntimeError, "Circular dependency detected: #{to_s} => #{value}"
      end
      self.class.new(value, self)
    end

    def to_s
      "#{prefix}#{@value}"
    end

    def self.append(value, chain)
      chain.append(value)
    end

    private

    def prefix
      "#{@tail.to_s} => "
    end

    class EmptyInvocationChain
      def member?(obj)
        false
      end
      def append(value)
        InvocationChain.new(value, self)
      end
      def to_s
        "TOP"
      end
    end

    EMPTY = EmptyInvocationChain.new
  end
end
