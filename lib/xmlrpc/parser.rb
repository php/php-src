#
# Parser for XML-RPC call and response
#
# Copyright (C) 2001, 2002, 2003 by Michael Neumann (mneumann@ntecs.de)
#
# $Id$
#


require "date"
require "xmlrpc/base64"
require "xmlrpc/datetime"


# add some methods to NQXML::Node
module NQXML
  class Node

    def removeChild(node)
      @children.delete(node)
    end
    def childNodes
      @children
    end
    def hasChildNodes
      not @children.empty?
    end
    def [] (index)
      @children[index]
    end

    def nodeType
      if @entity.instance_of? NQXML::Text then :TEXT
      elsif @entity.instance_of? NQXML::Comment then :COMMENT
      #elsif @entity.instance_of? NQXML::Element then :ELEMENT
      elsif @entity.instance_of? NQXML::Tag then :ELEMENT
      else :ELSE
      end
    end

    def nodeValue
      #TODO: error when wrong Entity-type
      @entity.text
    end
    def nodeName
      #TODO: error when wrong Entity-type
      @entity.name
    end
  end # class Node
end # module NQXML

module XMLRPC

  class FaultException < StandardError
    attr_reader :faultCode, :faultString

    def initialize(faultCode, faultString)
      @faultCode   = faultCode
      @faultString = faultString
      super(@faultString)
    end

    # returns a hash
    def to_h
      {"faultCode" => @faultCode, "faultString" => @faultString}
    end
  end

  module Convert
    def self.int(str)
      str.to_i
    end

    def self.boolean(str)
      case str
      when "0" then false
      when "1" then true
      else
        raise "RPC-value of type boolean is wrong"
      end
    end

    def self.double(str)
      str.to_f
    end

    def self.dateTime(str)
      case str
      when /^(-?\d\d\d\d)-?(\d\d)-?(\d\d)T(\d\d):(\d\d):(\d\d)(?:Z|([+-])(\d\d):?(\d\d))?$/
        a = [$1, $2, $3, $4, $5, $6].collect{|i| i.to_i}
        if $7
          ofs = $8.to_i*3600 + $9.to_i*60
          ofs = -ofs if $7=='+'
          utc = Time.utc(*a) + ofs
          a = [ utc.year, utc.month, utc.day, utc.hour, utc.min, utc.sec ]
        end
        XMLRPC::DateTime.new(*a)
      when /^(-?\d\d)-?(\d\d)-?(\d\d)T(\d\d):(\d\d):(\d\d)(Z|([+-]\d\d):(\d\d))?$/
        a = [$1, $2, $3, $4, $5, $6].collect{|i| i.to_i}
        if a[0] < 70
          a[0] += 2000
        else
          a[0] += 1900
        end
        if $7
          ofs = $8.to_i*3600 + $9.to_i*60
          ofs = -ofs if $7=='+'
          utc = Time.utc(*a) + ofs
          a = [ utc.year, utc.month, utc.day, utc.hour, utc.min, utc.sec ]
        end
        XMLRPC::DateTime.new(*a)
      else
        raise "wrong dateTime.iso8601 format " + str
      end
    end

    def self.base64(str)
      XMLRPC::Base64.decode(str)
    end

    def self.struct(hash)
      # convert to marhalled object
      klass = hash["___class___"]
      if klass.nil? or Config::ENABLE_MARSHALLING == false
        hash
      else
        begin
          mod = Module
          klass.split("::").each {|const| mod = mod.const_get(const.strip)}

          obj = mod.allocate

          hash.delete "___class___"
          hash.each {|key, value|
            obj.instance_variable_set("@#{ key }", value) if key =~ /^([a-zA-Z_]\w*)$/
          }
          obj
        rescue
          hash
        end
      end
    end

    def self.fault(hash)
      if hash.kind_of? Hash and hash.size == 2 and
        hash.has_key? "faultCode" and hash.has_key? "faultString" and
        hash["faultCode"].kind_of? Integer and hash["faultString"].kind_of? String

        XMLRPC::FaultException.new(hash["faultCode"], hash["faultString"])
      else
        raise "wrong fault-structure: #{hash.inspect}"
      end
    end

  end # module Convert

  module XMLParser

    class AbstractTreeParser

      def parseMethodResponse(str)
        methodResponse_document(createCleanedTree(str))
      end

      def parseMethodCall(str)
        methodCall_document(createCleanedTree(str))
      end

      private

      #
      # remove all whitespaces but in the tags i4, i8, int, boolean....
      # and all comments
      #
      def removeWhitespacesAndComments(node)
        remove = []
        childs = node.childNodes.to_a
        childs.each do |nd|
          case _nodeType(nd)
          when :TEXT
            # TODO: add nil?
            unless %w(i4 i8 int boolean string double dateTime.iso8601 base64).include? node.nodeName

               if node.nodeName == "value"
                 if not node.childNodes.to_a.detect {|n| _nodeType(n) == :ELEMENT}.nil?
                   remove << nd if nd.nodeValue.strip == ""
                 end
               else
                 remove << nd if nd.nodeValue.strip == ""
               end
            end
          when :COMMENT
            remove << nd
          else
            removeWhitespacesAndComments(nd)
          end
        end

        remove.each { |i| node.removeChild(i) }
      end


      def nodeMustBe(node, name)
        cmp = case name
        when Array
          name.include?(node.nodeName)
        when String
          name == node.nodeName
        else
          raise "error"
        end

        if not cmp then
          raise "wrong xml-rpc (name)"
        end

        node
      end

      #
      # returns, when successfully the only child-node
      #
      def hasOnlyOneChild(node, name=nil)
        if node.childNodes.to_a.size != 1
          raise "wrong xml-rpc (size)"
        end
        if name != nil then
          nodeMustBe(node.firstChild, name)
        end
      end


      def assert(b)
        if not b then
          raise "assert-fail"
        end
      end

      # the node `node` has empty string or string
      def text_zero_one(node)
        nodes = node.childNodes.to_a.size

        if nodes == 1
          text(node.firstChild)
        elsif nodes == 0
          ""
        else
          raise "wrong xml-rpc (size)"
        end
      end


      def integer(node)
        #TODO: check string for float because to_i returnsa
        #      0 when wrong string
         nodeMustBe(node, %w(i4 i8 int))
        hasOnlyOneChild(node)

        Convert.int(text(node.firstChild))
      end

      def boolean(node)
        nodeMustBe(node, "boolean")
        hasOnlyOneChild(node)

        Convert.boolean(text(node.firstChild))
      end

      def v_nil(node)
        nodeMustBe(node, "nil")
        assert( node.childNodes.to_a.size == 0 )
        nil
      end

      def string(node)
        nodeMustBe(node, "string")
        text_zero_one(node)
      end

      def double(node)
        #TODO: check string for float because to_f returnsa
        #      0.0 when wrong string
        nodeMustBe(node, "double")
        hasOnlyOneChild(node)

        Convert.double(text(node.firstChild))
      end

      def dateTime(node)
        nodeMustBe(node, "dateTime.iso8601")
        hasOnlyOneChild(node)

        Convert.dateTime( text(node.firstChild) )
      end

      def base64(node)
        nodeMustBe(node, "base64")
        #hasOnlyOneChild(node)

        Convert.base64(text_zero_one(node))
      end

      def member(node)
        nodeMustBe(node, "member")
        assert( node.childNodes.to_a.size == 2 )

        [ name(node[0]), value(node[1]) ]
      end

      def name(node)
        nodeMustBe(node, "name")
        #hasOnlyOneChild(node)
        text_zero_one(node)
      end

      def array(node)
        nodeMustBe(node, "array")
        hasOnlyOneChild(node, "data")
        data(node.firstChild)
      end

      def data(node)
        nodeMustBe(node, "data")

        node.childNodes.to_a.collect do |val|
          value(val)
        end
      end

      def param(node)
        nodeMustBe(node, "param")
        hasOnlyOneChild(node, "value")
        value(node.firstChild)
      end

      def methodResponse(node)
        nodeMustBe(node, "methodResponse")
        hasOnlyOneChild(node, %w(params fault))
        child = node.firstChild

        case child.nodeName
        when "params"
          [ true, params(child,false) ]
        when "fault"
          [ false, fault(child) ]
        else
          raise "unexpected error"
        end

      end

      def methodName(node)
        nodeMustBe(node, "methodName")
        hasOnlyOneChild(node)
        text(node.firstChild)
      end

      def params(node, call=true)
        nodeMustBe(node, "params")

        if call
          node.childNodes.to_a.collect do |n|
            param(n)
          end
        else # response (only one param)
          hasOnlyOneChild(node)
          param(node.firstChild)
        end
      end

      def fault(node)
        nodeMustBe(node, "fault")
        hasOnlyOneChild(node, "value")
        f = value(node.firstChild)
        Convert.fault(f)
      end



      # _nodeType is defined in the subclass
      def text(node)
        assert( _nodeType(node) == :TEXT )
        assert( node.hasChildNodes == false )
        assert( node.nodeValue != nil )

        node.nodeValue.to_s
      end

      def struct(node)
        nodeMustBe(node, "struct")

        hash = {}
        node.childNodes.to_a.each do |me|
          n, v = member(me)
          hash[n] = v
        end

        Convert.struct(hash)
      end


      def value(node)
        nodeMustBe(node, "value")
        nodes = node.childNodes.to_a.size
        if nodes == 0
          return ""
        elsif nodes > 1
          raise "wrong xml-rpc (size)"
        end

        child = node.firstChild

        case _nodeType(child)
        when :TEXT
          text_zero_one(node)
        when :ELEMENT
          case child.nodeName
          when "i4", "i8", "int"  then integer(child)
          when "boolean"          then boolean(child)
          when "string"           then string(child)
          when "double"           then double(child)
          when "dateTime.iso8601" then dateTime(child)
          when "base64"           then base64(child)
          when "struct"           then struct(child)
          when "array"            then array(child)
          when "nil"
            if Config::ENABLE_NIL_PARSER
              v_nil(child)
            else
              raise "wrong/unknown XML-RPC type 'nil'"
            end
          else
            raise "wrong/unknown XML-RPC type"
          end
        else
          raise "wrong type of node"
        end

      end

      def methodCall(node)
        nodeMustBe(node, "methodCall")
        assert( (1..2).include?( node.childNodes.to_a.size ) )
        name = methodName(node[0])

        if node.childNodes.to_a.size == 2 then
          pa = params(node[1])
        else # no parameters given
          pa = []
        end
        [name, pa]
      end

    end # module TreeParserMixin

    class AbstractStreamParser
      def parseMethodResponse(str)
        parser = @parser_class.new
        parser.parse(str)
        raise "No valid method response!" if parser.method_name != nil
        if parser.fault != nil
          # is a fault structure
          [false, parser.fault]
        else
          # is a normal return value
          raise "Missing return value!" if parser.params.size == 0
          raise "Too many return values. Only one allowed!" if parser.params.size > 1
          [true, parser.params[0]]
        end
      end

      def parseMethodCall(str)
        parser = @parser_class.new
        parser.parse(str)
        raise "No valid method call - missing method name!" if parser.method_name.nil?
        [parser.method_name, parser.params]
      end
    end

    module StreamParserMixin
      attr_reader :params
      attr_reader :method_name
      attr_reader :fault

      def initialize(*a)
        super(*a)
        @params = []
        @values = []
        @val_stack = []

        @names = []
        @name = []

        @structs = []
        @struct = {}

        @method_name = nil
        @fault = nil

        @data = nil
      end

      def startElement(name, attrs=[])
        @data = nil
        case name
        when "value"
          @value = nil
        when "nil"
          raise "wrong/unknown XML-RPC type 'nil'" unless Config::ENABLE_NIL_PARSER
          @value = :nil
        when "array"
          @val_stack << @values
          @values = []
        when "struct"
          @names << @name
          @name = []

          @structs << @struct
          @struct = {}
        end
      end

      def endElement(name)
        @data ||= ""
        case name
        when "string"
          @value = @data
        when "i4", "i8", "int"
          @value = Convert.int(@data)
        when "boolean"
          @value = Convert.boolean(@data)
        when "double"
          @value = Convert.double(@data)
        when "dateTime.iso8601"
          @value = Convert.dateTime(@data)
        when "base64"
          @value = Convert.base64(@data)
        when "value"
          @value = @data if @value.nil?
          @values << (@value == :nil ? nil : @value)
        when "array"
          @value = @values
          @values = @val_stack.pop
        when "struct"
          @value = Convert.struct(@struct)

          @name = @names.pop
          @struct = @structs.pop
        when "name"
          @name[0] = @data
        when "member"
          @struct[@name[0]] = @values.pop

        when "param"
          @params << @values[0]
          @values = []

        when "fault"
          @fault = Convert.fault(@values[0])

        when "methodName"
          @method_name = @data
        end

        @data = nil
      end

      def character(data)
        if @data
          @data << data
        else
          @data = data
        end
      end

    end # module StreamParserMixin

    # ---------------------------------------------------------------------------
    class XMLStreamParser < AbstractStreamParser
      def initialize
        require "xmlparser"
        @parser_class = Class.new(::XMLParser) {
          include StreamParserMixin
        }
      end
    end # class XMLStreamParser
    # ---------------------------------------------------------------------------
    class NQXMLStreamParser < AbstractStreamParser
      def initialize
        require "nqxml/streamingparser"
        @parser_class = XMLRPCParser
      end

      class XMLRPCParser
        include StreamParserMixin

        def parse(str)
          parser = NQXML::StreamingParser.new(str)
          parser.each do |ele|
            case ele
            when NQXML::Text
              @data = ele.text
              #character(ele.text)
            when NQXML::Tag
              if ele.isTagEnd
                endElement(ele.name)
              else
                startElement(ele.name, ele.attrs)
              end
            end
          end # do
        end # method parse
      end # class XMLRPCParser

    end # class NQXMLStreamParser
    # ---------------------------------------------------------------------------
    class XMLTreeParser < AbstractTreeParser

      def initialize
        require "xmltreebuilder"

        # The new XMLParser library (0.6.2+) uses a slightly different DOM implementation.
        # The following code removes the differences between both versions.
        if defined? XML::DOM::Builder
          return if defined? XML::DOM::Node::DOCUMENT # code below has been already executed
          klass = XML::DOM::Node
          klass.const_set("DOCUMENT", klass::DOCUMENT_NODE)
          klass.const_set("TEXT", klass::TEXT_NODE)
          klass.const_set("COMMENT", klass::COMMENT_NODE)
          klass.const_set("ELEMENT", klass::ELEMENT_NODE)
        end
      end

      private

      def _nodeType(node)
        tp = node.nodeType
        if tp == XML::SimpleTree::Node::TEXT then :TEXT
        elsif tp == XML::SimpleTree::Node::COMMENT then :COMMENT
        elsif tp == XML::SimpleTree::Node::ELEMENT then :ELEMENT
        else :ELSE
        end
      end


      def methodResponse_document(node)
        assert( node.nodeType == XML::SimpleTree::Node::DOCUMENT )
        hasOnlyOneChild(node, "methodResponse")

        methodResponse(node.firstChild)
      end

      def methodCall_document(node)
        assert( node.nodeType == XML::SimpleTree::Node::DOCUMENT )
        hasOnlyOneChild(node, "methodCall")

        methodCall(node.firstChild)
      end

      def createCleanedTree(str)
        doc = XML::SimpleTreeBuilder.new.parse(str)
        doc.documentElement.normalize
        removeWhitespacesAndComments(doc)
        doc
      end

    end # class XMLParser
    # ---------------------------------------------------------------------------
    class NQXMLTreeParser < AbstractTreeParser

      def initialize
        require "nqxml/treeparser"
      end

      private

      def _nodeType(node)
        node.nodeType
      end

      def methodResponse_document(node)
        methodResponse(node)
      end

      def methodCall_document(node)
        methodCall(node)
      end

      def createCleanedTree(str)
        doc = ::NQXML::TreeParser.new(str).document.rootNode
        removeWhitespacesAndComments(doc)
        doc
      end

    end # class NQXMLTreeParser
    # ---------------------------------------------------------------------------
    class REXMLStreamParser < AbstractStreamParser
      def initialize
        require "rexml/document"
        @parser_class = StreamListener
      end

      class StreamListener
        include StreamParserMixin

        alias :tag_start :startElement
        alias :tag_end :endElement
        alias :text :character
        alias :cdata :character

        def method_missing(*a)
          # ignore
        end

        def parse(str)
          REXML::Document.parse_stream(str, self)
        end
      end

    end
    # ---------------------------------------------------------------------------
    class XMLScanStreamParser < AbstractStreamParser
      def initialize
        require "xmlscan/parser"
        @parser_class = XMLScanParser
      end

      class XMLScanParser
        include StreamParserMixin

        Entities = {
          "lt"   => "<",
          "gt"   => ">",
          "amp"  => "&",
          "quot" => '"',
          "apos" => "'"
        }

        def parse(str)
          parser  = XMLScan::XMLParser.new(self)
          parser.parse(str)
        end

        alias :on_stag :startElement
        alias :on_etag :endElement

        def on_stag_end(name); end

        def on_stag_end_empty(name)
          startElement(name)
          endElement(name)
        end

        def on_chardata(str)
          character(str)
        end

        def on_cdata(str)
          character(str)
        end

        def on_entityref(ent)
          str = Entities[ent]
          if str
            character(str)
          else
            raise "unknown entity"
          end
        end

        def on_charref(code)
          character(code.chr)
        end

        def on_charref_hex(code)
          character(code.chr)
        end

        def method_missing(*a)
        end

        # TODO: call/implement?
        # valid_name?
        # valid_chardata?
        # valid_char?
        # parse_error

      end
    end
    # ---------------------------------------------------------------------------
    XMLParser   = XMLTreeParser
    NQXMLParser = NQXMLTreeParser

    Classes = [XMLStreamParser, XMLTreeParser,
               NQXMLStreamParser, NQXMLTreeParser,
               REXMLStreamParser, XMLScanStreamParser]

    # yields an instance of each installed parser
    def self.each_installed_parser
      XMLRPC::XMLParser::Classes.each do |klass|
        begin
          yield klass.new
        rescue LoadError
        end
      end
    end

  end # module XMLParser


end # module XMLRPC

