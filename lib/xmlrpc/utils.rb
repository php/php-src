#
# Defines ParserWriterChooseMixin, which makes it possible to choose a
# different XML writer and/or XML parser then the default one.
# The Mixin is used in client.rb (class Client) and server.rb (class
# BasicServer)
#
# Copyright (C) 2001, 2002, 2003 by Michael Neumann (mneumann@ntecs.de)
#
# $Id$
#

module XMLRPC

  #
  # This module enables a user-class to be marshalled
  # by XML-RPC for Ruby into a Hash, with one additional
  # key/value pair "___class___" => ClassName
  #
  module Marshallable
  end


  module ParserWriterChooseMixin

    def set_writer(writer)
      @create = Create.new(writer)
      self
    end

    def set_parser(parser)
      @parser = parser
      self
    end

    private

    def create
      # if set_writer was not already called then call it now
      if @create.nil? then
        set_writer(Config::DEFAULT_WRITER.new)
      end
      @create
    end

    def parser
      # if set_parser was not already called then call it now
      if @parser.nil? then
        set_parser(Config::DEFAULT_PARSER.new)
      end
      @parser
    end

  end # module ParserWriterChooseMixin


  module Service

  #
  # base class for Service Interface definitions, used
  # by BasicServer#add_handler
  #

  class BasicInterface
    attr_reader :prefix, :methods

    def initialize(prefix)
      @prefix = prefix
      @methods = []
    end

    def add_method(sig, help=nil, meth_name=nil)
      mname = nil
      sig = [sig] if sig.kind_of? String

      sig = sig.collect do |s|
        name, si = parse_sig(s)
        raise "Wrong signatures!" if mname != nil and name != mname
        mname = name
        si
      end

      @methods << [mname, meth_name || mname, sig, help]
    end

    private # ---------------------------------

    def parse_sig(sig)
      # sig is a String
      if sig =~ /^\s*(\w+)\s+([^(]+)(\(([^)]*)\))?\s*$/
        params = [$1]
        name   = $2.strip
        $4.split(",").each {|i| params << i.strip} if $4 != nil
        return name, params
      else
        raise "Syntax error in signature"
      end
    end

  end # class BasicInterface

  #
  # class which wraps a Service Interface definition, used
  # by BasicServer#add_handler
  #
  class Interface < BasicInterface
    def initialize(prefix, &p)
      raise "No interface specified" if p.nil?
      super(prefix)
      instance_eval(&p)
    end

    def get_methods(obj, delim=".")
      prefix = @prefix + delim
      @methods.collect { |name, meth, sig, help|
        [prefix + name.to_s, obj.method(meth).to_proc, sig, help]
      }
    end

    private # ---------------------------------

    def meth(*a)
      add_method(*a)
    end

  end # class Interface

  class PublicInstanceMethodsInterface < BasicInterface
    def initialize(prefix)
      super(prefix)
    end

    def get_methods(obj, delim=".")
      prefix = @prefix + delim
      obj.class.public_instance_methods(false).collect { |name|
        [prefix + name.to_s, obj.method(name).to_proc, nil, nil]
      }
    end
  end


  end # module Service


  #
  # short-form to create a Service::Interface
  #
  def self.interface(prefix, &p)
    Service::Interface.new(prefix, &p)
  end

  # short-cut for creating a PublicInstanceMethodsInterface
  def self.iPIMethods(prefix)
    Service::PublicInstanceMethodsInterface.new(prefix)
  end


  module ParseContentType
    def parse_content_type(str)
      a, *b = str.split(";")
      return a.strip.downcase, *b
    end
  end

end # module XMLRPC

