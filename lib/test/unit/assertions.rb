require 'minitest/unit'
require 'pp'

module Test
  module Unit
    module Assertions
      include MiniTest::Assertions

      def mu_pp(obj) #:nodoc:
        obj.pretty_inspect.chomp
      end

      MINI_DIR = File.join(File.dirname(File.dirname(File.expand_path(__FILE__))), "minitest") #:nodoc:

      UNASSIGNED = Object.new # :nodoc:

      # :call-seq:
      #   assert( test, failure_message = UNASSIGNED )
      #
      #Tests if +test+ is true.
      #
      #+msg+ may be a String or a Proc. If +msg+ is a String, it will be used
      #as the failure message. Otherwise, the result of calling +msg+ will be
      #used as the message if the assertion fails.
      #
      #If no +msg+ is given, a default message will be used.
      #
      #    assert(false, "This was expected to be true")
      def assert(test, msg = UNASSIGNED)
        case msg
        when UNASSIGNED
          msg = nil
        when String, Proc
        else
          bt = caller.reject { |s| s.rindex(MINI_DIR, 0) }
          raise ArgumentError, "assertion message must be String or Proc, but #{msg.class} was given.", bt
        end
        super
      end

      # :call-seq:
      #   assert_block( failure_message = nil )
      #
      #Tests the result of the given block. If the block does not return true,
      #the assertion will fail. The optional +failure_message+ argument is the same as in
      #Assertions#assert.
      #
      #    assert_block do
      #      [1, 2, 3].any? { |num| num < 1 }
      #    end
      def assert_block(*msgs)
        assert yield, *msgs
      end

      # :call-seq:
      #   assert_raise( *args, &block )
      #
      #Tests if the given block raises an exception. Acceptable exception
      #types maye be given as optional arguments. If the last argument is a
      #String, it will be used as the error message.
      #
      #    assert_raise do #Fails, no Exceptions are raised
      #    end
      #
      #    assert_raise NameError do
      #      puts x  #Raises NameError, so assertion succeeds
      #    end
      def assert_raise(*args, &b)
        assert_raises(*args, &b)
      end

      # :call-seq:
      #   assert_nothing_raised( *args, &block )
      #
      #If any exceptions are given as arguments, the assertion will
      #fail if one of those exceptions are raised. Otherwise, the test fails
      #if any exceptions are raised.
      #
      #The final argument may be a failure message.
      #
      #    assert_nothing_raised RuntimeError do
      #      raise Exception #Assertion passes, Exception is not a RuntimeError
      #    end
      #
      #    assert_nothing_raised do
      #      raise Exception #Assertion fails
      #    end
      def assert_nothing_raised(*args)
        self._assertions += 1
        if Module === args.last
          msg = nil
        else
          msg = args.pop
        end
        begin
          line = __LINE__; yield
        rescue MiniTest::Skip
          raise
        rescue Exception => e
          bt = e.backtrace
          as = e.instance_of?(MiniTest::Assertion)
          if as
            ans = /\A#{Regexp.quote(__FILE__)}:#{line}:in /o
            bt.reject! {|ln| ans =~ ln}
          end
          if ((args.empty? && !as) ||
              args.any? {|a| a.instance_of?(Module) ? e.is_a?(a) : e.class == a })
            msg = message(msg) { "Exception raised:\n<#{mu_pp(e)}>" }
            raise MiniTest::Assertion, msg.call, bt
          else
            raise
          end
        end
        nil
      end

      # :call-seq:
      #   assert_nothing_thrown( failure_message = nil, &block )
      #
      #Fails if the given block uses a call to Kernel#throw.
      #
      #An optional failure message may be provided as the final argument.
      #
      #    assert_nothing_thrown "Something was thrown!" do
      #      throw :problem?
      #    end
      def assert_nothing_thrown(msg=nil)
        begin
          yield
        rescue ArgumentError => error
          raise error if /\Auncaught throw (.+)\z/m !~ error.message
          msg = message(msg) { "<#{$1}> was thrown when nothing was expected" }
          flunk(msg)
        end
        assert(true, "Expected nothing to be thrown")
      end

      # :call-seq:
      #   assert_equal( expected, actual, failure_message = nil )
      #
      #Tests if +expected+ is equal to +actual+.
      #
      #An optional failure message may be provided as the final argument.
      def assert_equal(exp, act, msg = nil)
        msg = message(msg) {
          exp_str = mu_pp(exp)
          act_str = mu_pp(act)
          exp_comment = ''
          act_comment = ''
          if exp_str == act_str
            if (exp.is_a?(String) && act.is_a?(String)) ||
               (exp.is_a?(Regexp) && act.is_a?(Regexp))
              exp_comment = " (#{exp.encoding})"
              act_comment = " (#{act.encoding})"
            elsif exp.is_a?(Float) && act.is_a?(Float)
              exp_str = "%\#.#{Float::DIG+2}g" % exp
              act_str = "%\#.#{Float::DIG+2}g" % act
            elsif exp.is_a?(Time) && act.is_a?(Time)
              if exp.subsec * 1000_000_000 == exp.nsec
                exp_comment = " (#{exp.nsec}[ns])"
              else
                exp_comment = " (subsec=#{exp.subsec})"
              end
              if act.subsec * 1000_000_000 == act.nsec
                act_comment = " (#{act.nsec}[ns])"
              else
                act_comment = " (subsec=#{act.subsec})"
              end
            elsif exp.class != act.class
              # a subclass of Range, for example.
              exp_comment = " (#{exp.class})"
              act_comment = " (#{act.class})"
            end
          elsif !Encoding.compatible?(exp_str, act_str)
            if exp.is_a?(String) && act.is_a?(String)
              exp_str = exp.dump
              act_str = act.dump
              exp_comment = " (#{exp.encoding})"
              act_comment = " (#{act.encoding})"
            else
              exp_str = exp_str.dump
              act_str = act_str.dump
            end
          end
          "<#{exp_str}>#{exp_comment} expected but was\n<#{act_str}>#{act_comment}"
        }
        assert(exp == act, msg)
      end

      # :call-seq:
      #   assert_not_nil( expression, failure_message = nil )
      #
      #Tests if +expression+ is not nil.
      #
      #An optional failure message may be provided as the final argument.
      def assert_not_nil(exp, msg=nil)
        msg = message(msg) { "<#{mu_pp(exp)}> expected to not be nil" }
        assert(!exp.nil?, msg)
      end

      # :call-seq:
      #   assert_not_equal( expected, actual, failure_message = nil )
      #
      #Tests if +expected+ is not equal to +actual+.
      #
      #An optional failure message may be provided as the final argument.
      def assert_not_equal(exp, act, msg=nil)
        msg = message(msg) { "<#{mu_pp(exp)}> expected to be != to\n<#{mu_pp(act)}>" }
        assert(exp != act, msg)
      end

      # :call-seq:
      #   assert_no_match( regexp, string, failure_message = nil )
      #
      #Tests if the given Regexp does not match a given String.
      #
      #An optional failure message may be provided as the final argument.
      def assert_no_match(regexp, string, msg=nil)
        assert_instance_of(Regexp, regexp, "The first argument to assert_no_match should be a Regexp.")
        self._assertions -= 1
        msg = message(msg) { "<#{mu_pp(regexp)}> expected to not match\n<#{mu_pp(string)}>" }
        assert(regexp !~ string, msg)
      end

      # :call-seq:
      #   assert_not_same( expected, actual, failure_message = nil )
      #
      #Tests if +expected+ is not the same object as +actual+.
      #This test uses Object#equal? to test equality.
      #
      #An optional failure message may be provided as the final argument.
      #
      #    assert_not_same("x", "x") #Succeeds
      def assert_not_same(expected, actual, message="")
        msg = message(msg) { build_message(message, <<EOT, expected, expected.__id__, actual, actual.__id__) }
<?>
with id <?> expected to not be equal\\? to
<?>
with id <?>.
EOT
        assert(!actual.equal?(expected), msg)
      end

      # :call-seq:
      #   assert_respond_to( object, method, failure_message = nil )
      #
      #Tests if the given Object responds to +method+.
      #
      #An optional failure message may be provided as the final argument.
      #
      #    assert_respond_to("hello", :reverse)  #Succeeds
      #    assert_respond_to("hello", :does_not_exist)  #Fails
      def assert_respond_to obj, meth, msg = nil
        #get rid of overcounting
        super if !caller[0].rindex(MINI_DIR, 0) || !obj.respond_to?(meth)
      end

      # :call-seq:
      #   assert_send( +send_array+, failure_message = nil )
      #
      # Passes if the method send returns a true value.
      #
      # +send_array+ is composed of:
      # * A receiver
      # * A method
      # * Arguments to the method
      #
      # Example:
      #   assert_send([[1, 2], :member?, 1]) # -> pass
      #   assert_send([[1, 2], :member?, 4]) # -> fail
      def assert_send send_ary, m = nil
        recv, msg, *args = send_ary
        m = message(m) {
          if args.empty?
            argsstr = ""
          else
            (argsstr = mu_pp(args)).sub!(/\A\[(.*)\]\z/m, '(\1)')
          end
          "Expected #{mu_pp(recv)}.#{msg}#{argsstr} to return true"
        }
        assert recv.__send__(msg, *args), m
      end

      # :call-seq:
      #   assert_not_send( +send_array+, failure_message = nil )
      #
      # Passes if the method send doesn't return a true value.
      #
      # +send_array+ is composed of:
      # * A receiver
      # * A method
      # * Arguments to the method
      #
      # Example:
      #   assert_not_send([[1, 2], :member?, 1]) # -> fail
      #   assert_not_send([[1, 2], :member?, 4]) # -> pass
      def assert_not_send send_ary, m = nil
        recv, msg, *args = send_ary
        m = message(m) {
          if args.empty?
            argsstr = ""
          else
            (argsstr = mu_pp(args)).sub!(/\A\[(.*)\]\z/m, '(\1)')
          end
          "Expected #{mu_pp(recv)}.#{msg}#{argsstr} to return false"
        }
        assert !recv.__send__(msg, *args), m
      end

      ms = instance_methods(true).map {|sym| sym.to_s }
      ms.grep(/\Arefute_/) do |m|
        mname = ('assert_not_' << m.to_s[/.*?_(.*)/, 1])
        alias_method(mname, m) unless ms.include? mname
      end
      alias assert_include assert_includes
      alias assert_not_include assert_not_includes

      def build_message(head, template=nil, *arguments) #:nodoc:
        template &&= template.chomp
        template.gsub(/\G((?:[^\\]|\\.)*?)(\\)?\?/) { $1 + ($2 ? "?" : mu_pp(arguments.shift)) }
      end
    end
  end
end
