module DifferentOFS
  module WithDifferentOFS
    def setup
      super
      @ofs, $, = $,, "-"
    end
    def teardown
      $, = @ofs
      super
    end
  end

  def self.extended(klass)
    super(klass)
    klass.const_set(:DifferentOFS, Class.new(klass).class_eval {include WithDifferentOFS}).name
  end
end
