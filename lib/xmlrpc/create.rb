#
# Creates XML-RPC call/response documents
#
# Copyright (C) 2001, 2002, 2003 by Michael Neumann (mneumann@ntecs.de)
#
# $Id$
#

require "date"
require "xmlrpc/base64"

module XMLRPC

  module XMLWriter

    class Abstract
      def ele(name, *children)
        element(name, nil, *children)
      end

      def tag(name, txt)
        element(name, nil, text(txt))
      end
    end


    class Simple < Abstract

      def document_to_str(doc)
        doc
      end

      def document(*params)
        params.join("")
      end

      def pi(name, *params)
        "<?#{name} " + params.join(" ") + " ?>"
      end

      def element(name, attrs, *children)
        raise "attributes not yet implemented" unless attrs.nil?
        if children.empty?
          "<#{name}/>"
        else
          "<#{name}>" + children.join("") + "</#{name}>"
        end
      end

      def text(txt)
        cleaned = txt.dup
        cleaned.gsub!(/&/, '&amp;')
        cleaned.gsub!(/</, '&lt;')
        cleaned.gsub!(/>/, '&gt;')
        cleaned
      end

    end # class Simple


    class XMLParser < Abstract

      def initialize
        require "xmltreebuilder"
      end

      def document_to_str(doc)
        doc.to_s
      end

      def document(*params)
        XML::SimpleTree::Document.new(*params)
      end

      def pi(name, *params)
        XML::SimpleTree::ProcessingInstruction.new(name, *params)
      end

      def element(name, attrs, *children)
        XML::SimpleTree::Element.new(name, attrs, *children)
      end

      def text(txt)
        XML::SimpleTree::Text.new(txt)
      end

    end # class XMLParser

    Classes = [Simple, XMLParser]

    # yields an instance of each installed XML writer
    def self.each_installed_writer
      XMLRPC::XMLWriter::Classes.each do |klass|
        begin
          yield klass.new
        rescue LoadError
        end
      end
    end

  end # module XMLWriter

  class Create

    def initialize(xml_writer = nil)
      @writer = xml_writer || Config::DEFAULT_WRITER.new
    end


    def methodCall(name, *params)
      name = name.to_s

      if name !~ /[a-zA-Z0-9_.:\/]+/
        raise ArgumentError, "Wrong XML-RPC method-name"
      end

      parameter = params.collect do |param|
        @writer.ele("param", conv2value(param))
      end

      tree = @writer.document(
               @writer.pi("xml", 'version="1.0"'),
               @writer.ele("methodCall",
                 @writer.tag("methodName", name),
                 @writer.ele("params", *parameter)
               )
             )

      @writer.document_to_str(tree) + "\n"
    end



    #
    # generates a XML-RPC methodResponse document
    #
    # if is_ret == false then the params array must
    # contain only one element, which is a structure
    # of a fault return-value.
    #
    # if is_ret == true then a normal
    # return-value of all the given params is created.
    #
    def methodResponse(is_ret, *params)

      if is_ret
        resp = params.collect do |param|
          @writer.ele("param", conv2value(param))
        end

        resp = [@writer.ele("params", *resp)]
      else
        if params.size != 1 or params[0] === XMLRPC::FaultException
          raise ArgumentError, "no valid fault-structure given"
        end
        resp = @writer.ele("fault", conv2value(params[0].to_h))
      end


      tree = @writer.document(
               @writer.pi("xml", 'version="1.0"'),
               @writer.ele("methodResponse", resp)
             )

      @writer.document_to_str(tree) + "\n"
    end



    #####################################
    private
    #####################################

    #
    # converts a Ruby object into
    # a XML-RPC <value> tag
    #
    def conv2value(param)

        val = case param
        when Fixnum, Bignum
          # XML-RPC's int is 32bit int, and Fixnum also may be beyond 32bit
          if Config::ENABLE_BIGINT
            @writer.tag("i4", param.to_s)
          else
            if param >= -(2**31) and param <= (2**31-1)
              @writer.tag("i4", param.to_s)
            else
              raise "Bignum is too big! Must be signed 32-bit integer!"
            end
          end
        when TrueClass, FalseClass
          @writer.tag("boolean", param ? "1" : "0")

        when Symbol
          @writer.tag("string", param.to_s)

        when String
          @writer.tag("string", param)

        when NilClass
          if Config::ENABLE_NIL_CREATE
            @writer.ele("nil")
          else
            raise "Wrong type NilClass. Not allowed!"
          end

        when Float
          raise "Wrong value #{param}. Not allowed!" unless param.finite?
          @writer.tag("double", param.to_s)

        when Struct
          h = param.members.collect do |key|
            value = param[key]
            @writer.ele("member",
              @writer.tag("name", key.to_s),
              conv2value(value)
            )
          end

          @writer.ele("struct", *h)

        when Hash
          # TODO: can a Hash be empty?

          h = param.collect do |key, value|
            @writer.ele("member",
              @writer.tag("name", key.to_s),
              conv2value(value)
            )
          end

          @writer.ele("struct", *h)

        when Array
          # TODO: can an Array be empty?
          a = param.collect {|v| conv2value(v) }

          @writer.ele("array",
            @writer.ele("data", *a)
          )

        when Time, Date, ::DateTime
          @writer.tag("dateTime.iso8601", param.strftime("%Y%m%dT%H:%M:%S"))

        when XMLRPC::DateTime
          @writer.tag("dateTime.iso8601",
            format("%.4d%02d%02dT%02d:%02d:%02d", *param.to_a))

        when XMLRPC::Base64
          @writer.tag("base64", param.encoded)

        else
          if Config::ENABLE_MARSHALLING and param.class.included_modules.include? XMLRPC::Marshallable
            # convert Ruby object into Hash
            ret = {"___class___" => param.class.name}
            param.instance_variables.each {|v|
              name = v[1..-1]
              val = param.instance_variable_get(v)

              if val.nil?
                ret[name] = val if Config::ENABLE_NIL_CREATE
              else
                ret[name] = val
              end
            }
            return conv2value(ret)
          else
            ok, pa = wrong_type(param)
            if ok
              return conv2value(pa)
            else
              raise "Wrong type!"
            end
          end
        end

        @writer.ele("value", val)
    end

    def wrong_type(value)
      false
    end


  end # class Create

end # module XMLRPC

