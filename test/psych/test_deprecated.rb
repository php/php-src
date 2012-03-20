require 'psych/helper'

module Psych
  class TestDeprecated < TestCase
    def teardown
      Psych.domain_types.clear
    end

    class QuickEmitter
      attr_reader :name
      attr_reader :value

      def initialize
        @name  = 'hello!!'
        @value = 'Friday!'
      end

      def to_yaml opts = {}
        Psych.quick_emit object_id, opts do |out|
          out.map taguri, to_yaml_style do |map|
            map.add 'name', @name
            map.add 'value', nil
          end
        end
      end
    end

    def setup
      @qe = QuickEmitter.new
    end

    def test_quick_emit
      qe2 = Psych.load @qe.to_yaml
      assert_equal @qe.name, qe2.name
      assert_instance_of QuickEmitter, qe2
      assert_nil qe2.value
    end

    def test_recursive_quick_emit
      hash  = { :qe => @qe }
      hash2 = Psych.load Psych.dump hash
      qe    = hash2[:qe]

      assert_equal @qe.name, qe.name
      assert_instance_of QuickEmitter, qe
      assert_nil qe.value
    end

    class QuickEmitterEncodeWith
      attr_reader :name
      attr_reader :value

      def initialize
        @name  = 'hello!!'
        @value = 'Friday!'
      end

      def encode_with coder
        coder.map do |map|
          map.add 'name', @name
          map.add 'value', nil
        end
      end

      def to_yaml opts = {}
        raise
      end
    end

    ###
    # An object that defines both to_yaml and encode_with should only call
    # encode_with.
    def test_recursive_quick_emit_encode_with
      qeew = QuickEmitterEncodeWith.new
      hash  = { :qe => qeew }
      hash2 = Psych.load Psych.dump hash
      qe    = hash2[:qe]

      assert_equal qeew.name, qe.name
      assert_instance_of QuickEmitterEncodeWith, qe
      assert_nil qe.value
    end

    class YamlInit
      attr_reader :name
      attr_reader :value

      def initialize
        @name  = 'hello!!'
        @value = 'Friday!'
      end

      def yaml_initialize tag, vals
        vals.each { |ivar, val| instance_variable_set "@#{ivar}", 'TGIF!' }
      end
    end

    def test_yaml_initialize
      hash  = { :yi => YamlInit.new }
      hash2 = Psych.load Psych.dump hash
      yi    = hash2[:yi]

      assert_equal 'TGIF!', yi.name
      assert_equal 'TGIF!', yi.value
      assert_instance_of YamlInit, yi
    end

    class YamlInitAndInitWith
      attr_reader :name
      attr_reader :value

      def initialize
        @name  = 'shaners'
        @value = 'Friday!'
      end

      def init_with coder
        coder.map.each { |ivar, val| instance_variable_set "@#{ivar}", 'TGIF!' }
      end

      def yaml_initialize tag, vals
        raise
      end
    end

    ###
    # An object that implements both yaml_initialize and init_with should not
    # receive the yaml_initialize call.
    def test_yaml_initialize_and_init_with
      hash  = { :yi => YamlInitAndInitWith.new }
      hash2 = Psych.load Psych.dump hash
      yi    = hash2[:yi]

      assert_equal 'TGIF!', yi.name
      assert_equal 'TGIF!', yi.value
      assert_instance_of YamlInitAndInitWith, yi
    end

    def test_coder_scalar
      coder = Psych::Coder.new 'foo'
      coder.scalar('tag', 'some string', :plain)
      assert_equal 'tag', coder.tag
      assert_equal 'some string', coder.scalar
      assert_equal :scalar, coder.type
    end

    class YamlAs
      psych_yaml_as 'helloworld' # this should be yaml_as but to avoid syck
    end

    def test_yaml_as
      assert_match(/helloworld/, Psych.dump(YamlAs.new))
    end

    def test_ruby_type
      types = []
      appender = lambda { |*args| types << args }

      Psych.add_ruby_type('foo', &appender)
      Psych.load <<-eoyml
- !ruby.yaml.org,2002/foo bar
      eoyml

      assert_equal [["tag:ruby.yaml.org,2002:foo", "bar"]], types
    end

    def test_detect_implicit
      assert_equal '', Psych.detect_implicit(nil)
      assert_equal '', Psych.detect_implicit(Object.new)
      assert_equal '', Psych.detect_implicit(1.2)
      assert_equal 'null', Psych.detect_implicit('')
      assert_equal 'string', Psych.detect_implicit('foo')
    end

    def test_private_type
      types = []
      Psych.add_private_type('foo') { |*args| types << args }
      Psych.load <<-eoyml
- !x-private:foo bar
      eoyml

      assert_equal [["x-private:foo", "bar"]], types
    end

    def test_tagurize
      assert_nil Psych.tagurize nil
      assert_equal Psych, Psych.tagurize(Psych)
      assert_equal 'tag:yaml.org,2002:foo', Psych.tagurize('foo')
    end

    def test_read_type_class
      things = Psych.read_type_class 'tag:yaml.org,2002:int:Psych::TestDeprecated::QuickEmitter', Object
      assert_equal 'int', things.first
      assert_equal Psych::TestDeprecated::QuickEmitter, things.last
    end

    def test_read_type_class_no_class
      things = Psych.read_type_class 'tag:yaml.org,2002:int', Object
      assert_equal 'int', things.first
      assert_equal Object, things.last
    end

    def test_object_maker
      thing = Psych.object_maker(Object, { 'a' => 'b', 'c' => 'd' })
      assert_instance_of(Object, thing)
      assert_equal 'b', thing.instance_variable_get(:@a)
      assert_equal 'd', thing.instance_variable_get(:@c)
    end
  end
end
