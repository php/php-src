#
#   forwardable.rb -
#       $Release Version: 1.1$
#       $Revision$
#       by Keiju ISHITSUKA(keiju@ishitsuka.com)
#       original definition by delegator.rb
#       Revised by Daniel J. Berger with suggestions from Florian Gross.
#
#       Documentation by James Edward Gray II and Gavin Sinclair
#
# == Introduction
#
# This library allows you delegate method calls to an object, on a method by
# method basis.
#
# == Notes
#
# Be advised, RDoc will not detect delegated methods.
#
# <b>forwardable.rb provides single-method delegation via the
# def_delegator() and def_delegators() methods.  For full-class
# delegation via DelegateClass(), see delegate.rb.</b>
#
# == Examples
#
# === Forwardable
#
# Forwardable makes building a new class based on existing work, with a proper
# interface, almost trivial.  We want to rely on what has come before obviously,
# but with delegation we can take just the methods we need and even rename them
# as appropriate.  In many cases this is preferable to inheritance, which gives
# us the entire old interface, even if much of it isn't needed.
#
#   class Queue
#     extend Forwardable
#
#     def initialize
#       @q = [ ]    # prepare delegate object
#     end
#
#     # setup preferred interface, enq() and deq()...
#     def_delegator :@q, :push, :enq
#     def_delegator :@q, :shift, :deq
#
#     # support some general Array methods that fit Queues well
#     def_delegators :@q, :clear, :first, :push, :shift, :size
#   end
#
#   q = Queue.new
#   q.enq 1, 2, 3, 4, 5
#   q.push 6
#
#   q.shift    # => 1
#   while q.size > 0
#     puts q.deq
#   end
#
#   q.enq "Ruby", "Perl", "Python"
#   puts q.first
#   q.clear
#   puts q.first
#
# <i>Prints:</i>
#
#   2
#   3
#   4
#   5
#   6
#   Ruby
#   nil
#
# SingleForwardable can be used to setup delegation at the object level as well.
#
#    printer = String.new
#    printer.extend SingleForwardable        # prepare object for delegation
#    printer.def_delegator "STDOUT", "puts"  # add delegation for STDOUT.puts()
#    printer.puts "Howdy!"
#
# Also, SingleForwardable can be use to Class or Module.
#
#    module Facade
#      extend SingleForwardable
#      def_delegator :Implementation, :service
#
#      class Implementation
#         def service...
#      end
#    end
#
# If you want to use both Forwardable and SingleForwardable, you can
# use methods def_instance_delegator and def_single_delegator, etc.
#
# If the object isn't a Module and Class, You can too extend
# Forwardable module.
#    printer = String.new
#    printer.extend Forwardable              # prepare object for delegation
#    printer.def_delegator "STDOUT", "puts"  # add delegation for STDOUT.puts()
#    printer.puts "Howdy!"
#
# <i>Prints:</i>
#
#    Howdy!

#
# The Forwardable module provides delegation of specified
# methods to a designated object, using the methods #def_delegator
# and #def_delegators.
#
# For example, say you have a class RecordCollection which
# contains an array <tt>@records</tt>.  You could provide the lookup method
# #record_number(), which simply calls #[] on the <tt>@records</tt>
# array, like this:
#
#   class RecordCollection
#     extend Forwardable
#     def_delegator :@records, :[], :record_number
#   end
#
# Further, if you wish to provide the methods #size, #<<, and #map,
# all of which delegate to @records, this is how you can do it:
#
#   class RecordCollection
#     # extend Forwardable, but we did that above
#     def_delegators :@records, :size, :<<, :map
#   end
#   f = Foo.new
#   f.printf ...
#   f.gets
#   f.content_at(1)
#
# Also see the example at forwardable.rb.

module Forwardable
  FORWARDABLE_VERSION = "1.1.0"

  @debug = nil
  class << self
    attr_accessor :debug
  end

  # Takes a hash as its argument.  The key is a symbol or an array of
  # symbols.  These symbols correspond to method names.  The value is
  # the accessor to which the methods will be delegated.
  #
  # :call-seq:
  #    delegate method => accessor
  #    delegate [method, method, ...] => accessor
  #
  def instance_delegate(hash)
    hash.each{ |methods, accessor|
      methods = [methods] unless methods.respond_to?(:each)
      methods.each{ |method|
        def_instance_delegator(accessor, method)
      }
    }
  end

  #
  # Shortcut for defining multiple delegator methods, but with no
  # provision for using a different name.  The following two code
  # samples have the same effect:
  #
  #   def_delegators :@records, :size, :<<, :map
  #
  #   def_delegator :@records, :size
  #   def_delegator :@records, :<<
  #   def_delegator :@records, :map
  #
  def def_instance_delegators(accessor, *methods)
    methods.delete("__send__")
    methods.delete("__id__")
    for method in methods
      def_instance_delegator(accessor, method)
    end
  end

  # Define +method+ as delegator instance method with an optional
  # alias name +ali+. Method calls to +ali+ will be delegated to
  # +accessor.method+. 
  #
  #   class MyQueue
  #     extend Forwardable
  #     attr_reader :queue
  #     def initialize
  #       @queue = []
  #     end
  #     
  #     def_delegator :@queue, :push, :mypush
  #   end
  #
  #   q = MyQueue.new
  #   q.mypush 42
  #   q.queue    #=> [42]
  #   q.push 23  #=> NoMethodError
  #
  def def_instance_delegator(accessor, method, ali = method)
    line_no = __LINE__; str = %{
      def #{ali}(*args, &block)
        begin
          #{accessor}.__send__(:#{method}, *args, &block)
        rescue Exception
          $@.delete_if{|s| %r"#{Regexp.quote(__FILE__)}"o =~ s} unless Forwardable::debug
          ::Kernel::raise
        end
      end
    }
    # If it's not a class or module, it's an instance
    begin
      module_eval(str, __FILE__, line_no)
    rescue
      instance_eval(str, __FILE__, line_no)
    end

  end

  alias delegate instance_delegate
  alias def_delegators def_instance_delegators
  alias def_delegator def_instance_delegator
end

#
# Usage of The SingleForwardable is like Fowadable module.
#
module SingleForwardable
  # Takes a hash as its argument.  The key is a symbol or an array of
  # symbols.  These symbols correspond to method names.  The value is
  # the accessor to which the methods will be delegated.
  #
  # :call-seq:
  #    delegate method => accessor
  #    delegate [method, method, ...] => accessor
  #
  def single_delegate(hash)
    hash.each{ |methods, accessor|
      methods = [methods] unless methods.respond_to?(:each)
      methods.each{ |method|
        def_single_delegator(accessor, method)
      }
    }
  end

  #
  # Shortcut for defining multiple delegator methods, but with no
  # provision for using a different name.  The following two code
  # samples have the same effect:
  #
  #   def_delegators :@records, :size, :<<, :map
  #
  #   def_delegator :@records, :size
  #   def_delegator :@records, :<<
  #   def_delegator :@records, :map
  #
  def def_single_delegators(accessor, *methods)
    methods.delete("__send__")
    methods.delete("__id__")
    for method in methods
      def_single_delegator(accessor, method)
    end
  end

  #
  # Defines a method _method_ which delegates to _obj_ (i.e. it calls
  # the method of the same name in _obj_).  If _new_name_ is
  # provided, it is used as the name for the delegate method.
  #
  def def_single_delegator(accessor, method, ali = method)
    str = %{
      def #{ali}(*args, &block)
        begin
          #{accessor}.__send__(:#{method}, *args, &block)
        rescue Exception
          $@.delete_if{|s| %r"#{Regexp.quote(__FILE__)}"o =~ s} unless Forwardable::debug
          ::Kernel::raise
        end
      end
    }

    instance_eval(str, __FILE__, __LINE__)
  end

  alias delegate single_delegate
  alias def_delegators def_single_delegators
  alias def_delegator def_single_delegator
end
