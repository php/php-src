#
#   notifier.rb - output methods used by irb
#   	$Release Version: 0.9.6$
#   	$Revision$
#   	by Keiju ISHITSUKA(keiju@ruby-lang.org)
#
# --
#
#
#

require "e2mmap"
require "irb/output-method"

module IRB
  module Notifier
    extend Exception2MessageMapper
    def_exception :ErrUndefinedNotifier,
      "undefined notifier level: %d is specified"
    def_exception :ErrUnrecognizedLevel,
      "unrecognized notifier level: %s is specified"

    def def_notifier(prefix = "", output_method = StdioOutputMethod.new)
      CompositeNotifier.new(prefix, output_method)
    end
    module_function :def_notifier

    class AbstractNotifier
      def initialize(prefix, base_notifier)
	@prefix = prefix
	@base_notifier = base_notifier
      end

      attr_reader :prefix

      def notify?
	true
      end

      def print(*opts)
	@base_notifier.print prefix, *opts if notify?
      end

      def printn(*opts)
	@base_notifier.printn prefix, *opts if notify?
      end

      def printf(format, *opts)
	@base_notifier.printf(prefix + format, *opts) if notify?
      end

      def puts(*objs)
	if notify?
	  @base_notifier.puts(*objs.collect{|obj| prefix + obj.to_s})
	end
      end

      def pp(*objs)
	if notify?
	  @base_notifier.ppx @prefix, *objs
	end
      end

      def ppx(prefix, *objs)
	if notify?
	  @base_notifier.ppx @prefix+prefix, *objs
	end
      end

      def exec_if
	yield(@base_notifier) if notify?
      end
    end

    class CompositeNotifier<AbstractNotifier
      def initialize(prefix, base_notifier)
	super

	@notifiers = [D_NOMSG]
	@level_notifier = D_NOMSG
      end

      attr_reader :notifiers

      def def_notifier(level, prefix = "")
	notifier = LeveledNotifier.new(self, level, prefix)
	@notifiers[level] = notifier
	notifier
      end

      attr_reader :level_notifier
      alias level level_notifier

      def level_notifier=(value)
	case value
	when AbstractNotifier
	  @level_notifier = value
	when Integer
	  l = @notifiers[value]
	  Notifier.Raise ErrUndefinedNotifer, value unless l
	  @level_notifier = l
	else
	  Notifier.Raise ErrUnrecognizedLevel, value unless l
	end
      end

      alias level= level_notifier=
    end

    class LeveledNotifier<AbstractNotifier
      include Comparable

      def initialize(base, level, prefix)
	super(prefix, base)

	@level = level
      end

      attr_reader :level

      def <=>(other)
	@level <=> other.level
      end

      def notify?
	@base_notifier.level >= self
      end
    end

    class NoMsgNotifier<LeveledNotifier
      def initialize
	@base_notifier = nil
	@level = 0
	@prefix = ""
      end

      def notify?
	false
      end
    end

    D_NOMSG = NoMsgNotifier.new
  end
end
