#
#   e2mmap.rb - for ruby 1.1
#       $Release Version: 2.0$
#       $Revision: 1.10 $
#       by Keiju ISHITSUKA
#
# --
#   Usage:
#
# U1)
#   class Foo
#     extend Exception2MessageMapper
#     def_e2message ExistingExceptionClass, "message..."
#     def_exception :NewExceptionClass, "message..."[, superclass]
#     ...
#   end
#
# U2)
#   module Error
#     extend Exception2MessageMapper
#     def_e2meggage ExistingExceptionClass, "message..."
#     def_exception :NewExceptionClass, "message..."[, superclass]
#     ...
#   end
#   class Foo
#     include Error
#     ...
#   end
#
#   foo = Foo.new
#   foo.Fail ....
#
# U3)
#   module Error
#     extend Exception2MessageMapper
#     def_e2message ExistingExceptionClass, "message..."
#     def_exception :NewExceptionClass, "message..."[, superclass]
#     ...
#   end
#   class Foo
#     extend Exception2MessageMapper
#     include Error
#     ...
#   end
#
#   Foo.Fail NewExceptionClass, arg...
#   Foo.Fail ExistingExceptionClass, arg...
#
#
module Exception2MessageMapper
  @RCS_ID='-$Id: e2mmap.rb,v 1.10 1999/02/17 12:33:17 keiju Exp keiju $-'

  E2MM = Exception2MessageMapper

  def E2MM.extend_object(cl)
    super
    cl.bind(self) unless cl < E2MM
  end

  def bind(cl)
    self.module_eval %[
      def Raise(err = nil, *rest)
        Exception2MessageMapper.Raise(self.class, err, *rest)
      end
      alias Fail Raise

      def self.included(mod)
        mod.extend Exception2MessageMapper
      end
    ]
  end

  # Fail(err, *rest)
  #     err:    exception
  #     rest:   message arguments
  #
  def Raise(err = nil, *rest)
    E2MM.Raise(self, err, *rest)
  end
  alias Fail Raise
  alias fail Raise

  # def_e2message(c, m)
  #         c:  exception
  #         m:  message_form
  #     define exception c with message m.
  #
  def def_e2message(c, m)
    E2MM.def_e2message(self, c, m)
  end

  # def_exception(n, m, s)
  #         n:  exception_name
  #         m:  message_form
  #         s:  superclass(default: StandardError)
  #     define exception named ``c'' with message m.
  #
  def def_exception(n, m, s = StandardError)
    E2MM.def_exception(self, n, m, s)
  end

  #
  # Private definitions.
  #
  # {[class, exp] => message, ...}
  @MessageMap = {}

  # E2MM.def_e2message(k, e, m)
  #         k:  class to define exception under.
  #         e:  exception
  #         m:  message_form
  #     define exception c with message m.
  #
  def E2MM.def_e2message(k, c, m)
    E2MM.instance_eval{@MessageMap[[k, c]] = m}
    c
  end

  # E2MM.def_exception(k, n, m, s)
  #         k:  class to define exception under.
  #         n:  exception_name
  #         m:  message_form
  #         s:  superclass(default: StandardError)
  #     define exception named ``c'' with message m.
  #
  def E2MM.def_exception(k, n, m, s = StandardError)
    n = n.id2name if n.kind_of?(Fixnum)
    e = Class.new(s)
    E2MM.instance_eval{@MessageMap[[k, e]] = m}
    k.const_set(n, e)
  end

  # Fail(klass, err, *rest)
  #     klass:  class to define exception under.
  #     err:    exception
  #     rest:   message arguments
  #
  def E2MM.Raise(klass = E2MM, err = nil, *rest)
    if form = e2mm_message(klass, err)
      b = $@.nil? ? caller(1) : $@
      #p $@
      #p __FILE__
      b.shift if b[0] =~ /^#{Regexp.quote(__FILE__)}:/
      raise err, sprintf(form, *rest), b
    else
      E2MM.Fail E2MM, ErrNotRegisteredException, err.inspect
    end
  end
  class << E2MM
    alias Fail Raise
  end

  def E2MM.e2mm_message(klass, exp)
    for c in klass.ancestors
      if mes = @MessageMap[[c,exp]]
        #p mes
        m = klass.instance_eval('"' + mes + '"')
        return m
      end
    end
    nil
  end
  class << self
    alias message e2mm_message
  end

  E2MM.def_exception(E2MM,
                     :ErrNotRegisteredException,
                     "not registerd exception(%s)")
end


