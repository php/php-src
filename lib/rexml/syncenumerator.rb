module REXML
  class SyncEnumerator
    include Enumerable

    # Creates a new SyncEnumerator which enumerates rows of given
    # Enumerable objects.
    def initialize(*enums)
      @gens = enums
      @length = @gens.collect {|x| x.size }.max
    end

    # Returns the number of enumerated Enumerable objects, i.e. the size
    # of each row.
    def size
      @gens.size
    end

    # Returns the number of enumerated Enumerable objects, i.e. the size
    # of each row.
    def length
      @gens.length
    end

    # Enumerates rows of the Enumerable objects.
    def each
      @length.times {|i|
        yield @gens.collect {|x| x[i]}
      }
      self
    end
  end
end
