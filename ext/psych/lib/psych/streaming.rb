module Psych
  module Streaming
    ###
    # Create a new streaming emitter.  Emitter will print to +io+.  See
    # Psych::Stream for an example.
    def initialize io
      super({}, self.class.const_get(:Emitter).new(io))
    end

    ###
    # Start streaming using +encoding+
    def start encoding = Nodes::Stream::UTF8
      super.tap { yield self if block_given?  }
    ensure
      finish if block_given?
    end

    private
    def register target, obj
    end
  end
end
