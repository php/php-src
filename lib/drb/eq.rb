module DRb
  class DRbObject
    def ==(other)
      return false unless DRbObject === other
     (@ref == other.__drbref) && (@uri == other.__drburi)
    end

    def hash
      [@uri, @ref].hash
    end

    alias eql? ==
  end
end
