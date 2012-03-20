module REXML
  # If you add a method, keep in mind two things:
  # (1) the first argument will always be a list of nodes from which to
  # filter.  In the case of context methods (such as position), the function
  # should return an array with a value for each child in the array.
  # (2) all method calls from XML will have "-" replaced with "_".
  # Therefore, in XML, "local-name()" is identical (and actually becomes)
  # "local_name()"
  module Functions
    @@context = nil
    @@namespace_context = {}
    @@variables = {}

    def Functions::namespace_context=(x) ; @@namespace_context=x ; end
    def Functions::variables=(x) ; @@variables=x ; end
    def Functions::namespace_context ; @@namespace_context ; end
    def Functions::variables ; @@variables ; end

    def Functions::context=(value); @@context = value; end

    def Functions::text( )
      if @@context[:node].node_type == :element
        return @@context[:node].find_all{|n| n.node_type == :text}.collect{|n| n.value}
      elsif @@context[:node].node_type == :text
        return @@context[:node].value
      else
        return false
      end
    end

    # Returns the last node of the given list of nodes.
    def Functions::last( )
      @@context[:size]
    end

    def Functions::position( )
      @@context[:index]
    end

    # Returns the size of the given list of nodes.
    def Functions::count( node_set )
      node_set.size
    end

    # Since REXML is non-validating, this method is not implemented as it
    # requires a DTD
    def Functions::id( object )
    end

    # UNTESTED
    def Functions::local_name( node_set=nil )
      get_namespace( node_set ) do |node|
        return node.local_name
      end
    end

    def Functions::namespace_uri( node_set=nil )
      get_namespace( node_set ) {|node| node.namespace}
    end

    def Functions::name( node_set=nil )
      get_namespace( node_set ) do |node|
        node.expanded_name
      end
    end

    # Helper method.
    def Functions::get_namespace( node_set = nil )
      if node_set == nil
        yield @@context[:node] if defined? @@context[:node].namespace
      else
        if node_set.respond_to? :each
          node_set.each { |node| yield node if defined? node.namespace }
        elsif node_set.respond_to? :namespace
          yield node_set
        end
      end
    end

    # A node-set is converted to a string by returning the string-value of the
    # node in the node-set that is first in document order. If the node-set is
    # empty, an empty string is returned.
    #
    # A number is converted to a string as follows
    #
    # NaN is converted to the string NaN
    #
    # positive zero is converted to the string 0
    #
    # negative zero is converted to the string 0
    #
    # positive infinity is converted to the string Infinity
    #
    # negative infinity is converted to the string -Infinity
    #
    # if the number is an integer, the number is represented in decimal form
    # as a Number with no decimal point and no leading zeros, preceded by a
    # minus sign (-) if the number is negative
    #
    # otherwise, the number is represented in decimal form as a Number
    # including a decimal point with at least one digit before the decimal
    # point and at least one digit after the decimal point, preceded by a
    # minus sign (-) if the number is negative; there must be no leading zeros
    # before the decimal point apart possibly from the one required digit
    # immediately before the decimal point; beyond the one required digit
    # after the decimal point there must be as many, but only as many, more
    # digits as are needed to uniquely distinguish the number from all other
    # IEEE 754 numeric values.
    #
    # The boolean false value is converted to the string false. The boolean
    # true value is converted to the string true.
    #
    # An object of a type other than the four basic types is converted to a
    # string in a way that is dependent on that type.
    def Functions::string( object=nil )
      #object = @context unless object
      if object.instance_of? Array
        string( object[0] )
      elsif defined? object.node_type
        if object.node_type == :attribute
          object.value
        elsif object.node_type == :element || object.node_type == :document
          string_value(object)
        else
          object.to_s
        end
      elsif object.nil?
        return ""
      else
        object.to_s
      end
    end

    # A node-set is converted to a string by
    # returning the concatenation of the string-value
    # of each of the children of the node in the
    # node-set that is first in document order.
    # If the node-set is empty, an empty string is returned.
    def Functions::string_value( o )
      rv = ""
      o.children.each { |e|
        if e.node_type == :text
          rv << e.to_s
        elsif e.node_type == :element
          rv << string_value( e )
        end
      }
      rv
    end

    # UNTESTED
    def Functions::concat( *objects )
      objects.join
    end

    # Fixed by Mike Stok
    def Functions::starts_with( string, test )
      string(string).index(string(test)) == 0
    end

    # Fixed by Mike Stok
    def Functions::contains( string, test )
      string(string).include?(string(test))
    end

    # Kouhei fixed this
    def Functions::substring_before( string, test )
      ruby_string = string(string)
      ruby_index = ruby_string.index(string(test))
      if ruby_index.nil?
        ""
      else
        ruby_string[ 0...ruby_index ]
      end
    end

    # Kouhei fixed this too
    def Functions::substring_after( string, test )
      ruby_string = string(string)
      return $1 if ruby_string =~ /#{test}(.*)/
      ""
    end

    # Take equal portions of Mike Stok and Sean Russell; mix
    # vigorously, and pour into a tall, chilled glass.  Serves 10,000.
    def Functions::substring( string, start, length=nil )
      ruby_string = string(string)
      ruby_length = if length.nil?
                      ruby_string.length.to_f
                    else
                      number(length)
                    end
      ruby_start = number(start)

      # Handle the special cases
      return '' if (
        ruby_length.nan? or
        ruby_start.nan? or
        ruby_start.infinite?
      )

      infinite_length = ruby_length.infinite? == 1
      ruby_length = ruby_string.length if infinite_length

      # Now, get the bounds.  The XPath bounds are 1..length; the ruby bounds
      # are 0..length.  Therefore, we have to offset the bounds by one.
      ruby_start = ruby_start.round - 1
      ruby_length = ruby_length.round

      if ruby_start < 0
       ruby_length += ruby_start unless infinite_length
       ruby_start = 0
      end
      return '' if ruby_length <= 0
      ruby_string[ruby_start,ruby_length]
    end

    # UNTESTED
    def Functions::string_length( string )
      string(string).length
    end

    # UNTESTED
    def Functions::normalize_space( string=nil )
      string = string(@@context[:node]) if string.nil?
      if string.kind_of? Array
        string.collect{|x| string.to_s.strip.gsub(/\s+/um, ' ') if string}
      else
        string.to_s.strip.gsub(/\s+/um, ' ')
      end
    end

    # This is entirely Mike Stok's beast
    def Functions::translate( string, tr1, tr2 )
      from = string(tr1)
      to = string(tr2)

      # the map is our translation table.
      #
      # if a character occurs more than once in the
      # from string then we ignore the second &
      # subsequent mappings
      #
      # if a character maps to nil then we delete it
      # in the output.  This happens if the from
      # string is longer than the to string
      #
      # there's nothing about - or ^ being special in
      # http://www.w3.org/TR/xpath#function-translate
      # so we don't build ranges or negated classes

      map = Hash.new
      0.upto(from.length - 1) { |pos|
        from_char = from[pos]
        unless map.has_key? from_char
          map[from_char] =
          if pos < to.length
            to[pos]
          else
            nil
          end
        end
      }

      if ''.respond_to? :chars
        string(string).chars.collect { |c|
          if map.has_key? c then map[c] else c end
        }.compact.join
      else
        string(string).unpack('U*').collect { |c|
          if map.has_key? c then map[c] else c end
        }.compact.pack('U*')
      end
    end

    # UNTESTED
    def Functions::boolean( object=nil )
      if object.kind_of? String
        if object =~ /\d+/u
          return object.to_f != 0
        else
          return object.size > 0
        end
      elsif object.kind_of? Array
        object = object.find{|x| x and true}
      end
      return object ? true : false
    end

    # UNTESTED
    def Functions::not( object )
      not boolean( object )
    end

    # UNTESTED
    def Functions::true( )
      true
    end

    # UNTESTED
    def Functions::false(  )
      false
    end

    # UNTESTED
    def Functions::lang( language )
      lang = false
      node = @@context[:node]
      attr = nil
      until node.nil?
        if node.node_type == :element
          attr = node.attributes["xml:lang"]
          unless attr.nil?
            lang = compare_language(string(language), attr)
            break
          else
          end
        end
        node = node.parent
      end
      lang
    end

    def Functions::compare_language lang1, lang2
      lang2.downcase.index(lang1.downcase) == 0
    end

    # a string that consists of optional whitespace followed by an optional
    # minus sign followed by a Number followed by whitespace is converted to
    # the IEEE 754 number that is nearest (according to the IEEE 754
    # round-to-nearest rule) to the mathematical value represented by the
    # string; any other string is converted to NaN
    #
    # boolean true is converted to 1; boolean false is converted to 0
    #
    # a node-set is first converted to a string as if by a call to the string
    # function and then converted in the same way as a string argument
    #
    # an object of a type other than the four basic types is converted to a
    # number in a way that is dependent on that type
    def Functions::number( object=nil )
      object = @@context[:node] unless object
      case object
      when true
        Float(1)
      when false
        Float(0)
      when Array
        number(string( object ))
      when Numeric
        object.to_f
      else
        str = string( object )
        # If XPath ever gets scientific notation...
        #if str =~ /^\s*-?(\d*\.?\d+|\d+\.)([Ee]\d*)?\s*$/
        if str =~ /^\s*-?(\d*\.?\d+|\d+\.)\s*$/
          str.to_f
        else
          (0.0 / 0.0)
        end
      end
    end

    def Functions::sum( nodes )
      nodes = [nodes] unless nodes.kind_of? Array
      nodes.inject(0) { |r,n| r += number(string(n)) }
    end

    def Functions::floor( number )
      number(number).floor
    end

    def Functions::ceiling( number )
      number(number).ceil
    end

    def Functions::round( number )
      begin
        number(number).round
      rescue FloatDomainError
        number(number)
      end
    end

    def Functions::processing_instruction( node )
      node.node_type == :processing_instruction
    end

    def Functions::method_missing( id )
      puts "METHOD MISSING #{id.id2name}"
      XPath.match( @@context[:node], id.id2name )
    end
  end
end
