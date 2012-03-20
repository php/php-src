require 'rexml/parseexception'
require 'rexml/undefinednamespaceexception'
require 'rexml/source'
require 'set'

module REXML
  module Parsers
    # = Using the Pull Parser
    # <em>This API is experimental, and subject to change.</em>
    #  parser = PullParser.new( "<a>text<b att='val'/>txet</a>" )
    #  while parser.has_next?
    #    res = parser.next
    #    puts res[1]['att'] if res.start_tag? and res[0] == 'b'
    #  end
    # See the PullEvent class for information on the content of the results.
    # The data is identical to the arguments passed for the various events to
    # the StreamListener API.
    #
    # Notice that:
    #  parser = PullParser.new( "<a>BAD DOCUMENT" )
    #  while parser.has_next?
    #    res = parser.next
    #    raise res[1] if res.error?
    #  end
    #
    # Nat Price gave me some good ideas for the API.
    class BaseParser
      LETTER = '[:alpha:]'
      DIGIT = '[:digit:]'

      COMBININGCHAR = '' # TODO
      EXTENDER = ''      # TODO

      NCNAME_STR= "[#{LETTER}_:][-[:alnum:]._:#{COMBININGCHAR}#{EXTENDER}]*"
      NAME_STR= "(?:(#{NCNAME_STR}):)?(#{NCNAME_STR})"
      UNAME_STR= "(?:#{NCNAME_STR}:)?#{NCNAME_STR}"

      NAMECHAR = '[\-\w\.:]'
      NAME = "([\\w:]#{NAMECHAR}*)"
      NMTOKEN = "(?:#{NAMECHAR})+"
      NMTOKENS = "#{NMTOKEN}(\\s+#{NMTOKEN})*"
      REFERENCE = "&(?:#{NAME};|#\\d+;|#x[0-9a-fA-F]+;)"
      REFERENCE_RE = /#{REFERENCE}/

      DOCTYPE_START = /\A\s*<!DOCTYPE\s/um
      DOCTYPE_PATTERN = /\s*<!DOCTYPE\s+(.*?)(\[|>)/um
      ATTRIBUTE_PATTERN = /\s*(#{NAME_STR})\s*=\s*(["'])(.*?)\4/um
      COMMENT_START = /\A<!--/u
      COMMENT_PATTERN = /<!--(.*?)-->/um
      CDATA_START = /\A<!\[CDATA\[/u
      CDATA_END = /^\s*\]\s*>/um
      CDATA_PATTERN = /<!\[CDATA\[(.*?)\]\]>/um
      XMLDECL_START = /\A<\?xml\s/u;
      XMLDECL_PATTERN = /<\?xml\s+(.*?)\?>/um
      INSTRUCTION_START = /\A<\?/u
      INSTRUCTION_PATTERN = /<\?(.*?)(\s+.*?)?\?>/um
      TAG_MATCH = /^<((?>#{NAME_STR}))\s*((?>\s+#{UNAME_STR}\s*=\s*(["']).*?\5)*)\s*(\/)?>/um
      CLOSE_MATCH = /^\s*<\/(#{NAME_STR})\s*>/um

      VERSION = /\bversion\s*=\s*["'](.*?)['"]/um
      ENCODING = /\bencoding\s*=\s*["'](.*?)['"]/um
      STANDALONE = /\bstandalone\s*=\s*["'](.*?)['"]/um

      ENTITY_START = /^\s*<!ENTITY/
      IDENTITY = /^([!\*\w\-]+)(\s+#{NCNAME_STR})?(\s+["'](.*?)['"])?(\s+['"](.*?)["'])?/u
      ELEMENTDECL_START = /^\s*<!ELEMENT/um
      ELEMENTDECL_PATTERN = /^\s*(<!ELEMENT.*?)>/um
      SYSTEMENTITY = /^\s*(%.*?;)\s*$/um
      ENUMERATION = "\\(\\s*#{NMTOKEN}(?:\\s*\\|\\s*#{NMTOKEN})*\\s*\\)"
      NOTATIONTYPE = "NOTATION\\s+\\(\\s*#{NAME}(?:\\s*\\|\\s*#{NAME})*\\s*\\)"
      ENUMERATEDTYPE = "(?:(?:#{NOTATIONTYPE})|(?:#{ENUMERATION}))"
      ATTTYPE = "(CDATA|ID|IDREF|IDREFS|ENTITY|ENTITIES|NMTOKEN|NMTOKENS|#{ENUMERATEDTYPE})"
      ATTVALUE = "(?:\"((?:[^<&\"]|#{REFERENCE})*)\")|(?:'((?:[^<&']|#{REFERENCE})*)')"
      DEFAULTDECL = "(#REQUIRED|#IMPLIED|(?:(#FIXED\\s+)?#{ATTVALUE}))"
      ATTDEF = "\\s+#{NAME}\\s+#{ATTTYPE}\\s+#{DEFAULTDECL}"
      ATTDEF_RE = /#{ATTDEF}/
      ATTLISTDECL_START = /^\s*<!ATTLIST/um
      ATTLISTDECL_PATTERN = /^\s*<!ATTLIST\s+#{NAME}(?:#{ATTDEF})*\s*>/um
      NOTATIONDECL_START = /^\s*<!NOTATION/um
      PUBLIC = /^\s*<!NOTATION\s+(\w[\-\w]*)\s+(PUBLIC)\s+(["'])(.*?)\3(?:\s+(["'])(.*?)\5)?\s*>/um
      SYSTEM = /^\s*<!NOTATION\s+(\w[\-\w]*)\s+(SYSTEM)\s+(["'])(.*?)\3\s*>/um

      TEXT_PATTERN = /\A([^<]*)/um

      # Entity constants
      PUBIDCHAR = "\x20\x0D\x0Aa-zA-Z0-9\\-()+,./:=?;!*@$_%#"
      SYSTEMLITERAL = %Q{((?:"[^"]*")|(?:'[^']*'))}
      PUBIDLITERAL = %Q{("[#{PUBIDCHAR}']*"|'[#{PUBIDCHAR}]*')}
      EXTERNALID = "(?:(?:(SYSTEM)\\s+#{SYSTEMLITERAL})|(?:(PUBLIC)\\s+#{PUBIDLITERAL}\\s+#{SYSTEMLITERAL}))"
      NDATADECL = "\\s+NDATA\\s+#{NAME}"
      PEREFERENCE = "%#{NAME};"
      ENTITYVALUE = %Q{((?:"(?:[^%&"]|#{PEREFERENCE}|#{REFERENCE})*")|(?:'([^%&']|#{PEREFERENCE}|#{REFERENCE})*'))}
      PEDEF = "(?:#{ENTITYVALUE}|#{EXTERNALID})"
      ENTITYDEF = "(?:#{ENTITYVALUE}|(?:#{EXTERNALID}(#{NDATADECL})?))"
      PEDECL = "<!ENTITY\\s+(%)\\s+#{NAME}\\s+#{PEDEF}\\s*>"
      GEDECL = "<!ENTITY\\s+#{NAME}\\s+#{ENTITYDEF}\\s*>"
      ENTITYDECL = /\s*(?:#{GEDECL})|(?:#{PEDECL})/um

      EREFERENCE = /&(?!#{NAME};)/

      DEFAULT_ENTITIES = {
        'gt' => [/&gt;/, '&gt;', '>', />/],
        'lt' => [/&lt;/, '&lt;', '<', /</],
        'quot' => [/&quot;/, '&quot;', '"', /"/],
        "apos" => [/&apos;/, "&apos;", "'", /'/]
      }


      ######################################################################
      # These are patterns to identify common markup errors, to make the
      # error messages more informative.
      ######################################################################
      MISSING_ATTRIBUTE_QUOTES = /^<#{NAME_STR}\s+#{NAME_STR}\s*=\s*[^"']/um

      def initialize( source )
        self.stream = source
        @listeners = []
      end

      def add_listener( listener )
        @listeners << listener
      end

      attr_reader :source

      def stream=( source )
        @source = SourceFactory.create_from( source )
        @closed = nil
        @document_status = nil
        @tags = []
        @stack = []
        @entities = []
        @nsstack = []
      end

      def position
        if @source.respond_to? :position
          @source.position
        else
          # FIXME
          0
        end
      end

      # Returns true if there are no more events
      def empty?
        return (@source.empty? and @stack.empty?)
      end

      # Returns true if there are more events.  Synonymous with !empty?
      def has_next?
        return !(@source.empty? and @stack.empty?)
      end

      # Push an event back on the head of the stream.  This method
      # has (theoretically) infinite depth.
      def unshift token
        @stack.unshift(token)
      end

      # Peek at the +depth+ event in the stack.  The first element on the stack
      # is at depth 0.  If +depth+ is -1, will parse to the end of the input
      # stream and return the last event, which is always :end_document.
      # Be aware that this causes the stream to be parsed up to the +depth+
      # event, so you can effectively pre-parse the entire document (pull the
      # entire thing into memory) using this method.
      def peek depth=0
        raise %Q[Illegal argument "#{depth}"] if depth < -1
        temp = []
        if depth == -1
          temp.push(pull()) until empty?
        else
          while @stack.size+temp.size < depth+1
            temp.push(pull())
          end
        end
        @stack += temp if temp.size > 0
        @stack[depth]
      end

      # Returns the next event.  This is a +PullEvent+ object.
      def pull
        pull_event.tap do |event|
          @listeners.each do |listener|
            listener.receive event
          end
        end
      end

      def pull_event
        if @closed
          x, @closed = @closed, nil
          return [ :end_element, x ]
        end
        return [ :end_document ] if empty?
        return @stack.shift if @stack.size > 0
        #STDERR.puts @source.encoding
        @source.read if @source.buffer.size<2
        #STDERR.puts "BUFFER = #{@source.buffer.inspect}"
        if @document_status == nil
          #@source.consume( /^\s*/um )
          word = @source.match( /^((?:\s+)|(?:<[^>]*>))/um )
          word = word[1] unless word.nil?
          #STDERR.puts "WORD = #{word.inspect}"
          case word
          when COMMENT_START
            return [ :comment, @source.match( COMMENT_PATTERN, true )[1] ]
          when XMLDECL_START
            #STDERR.puts "XMLDECL"
            results = @source.match( XMLDECL_PATTERN, true )[1]
            version = VERSION.match( results )
            version = version[1] unless version.nil?
            encoding = ENCODING.match(results)
            encoding = encoding[1] unless encoding.nil?
            @source.encoding = encoding
            standalone = STANDALONE.match(results)
            standalone = standalone[1] unless standalone.nil?
            return [ :xmldecl, version, encoding, standalone ]
          when INSTRUCTION_START
            return [ :processing_instruction, *@source.match(INSTRUCTION_PATTERN, true)[1,2] ]
          when DOCTYPE_START
            md = @source.match( DOCTYPE_PATTERN, true )
            @nsstack.unshift(curr_ns=Set.new)
            identity = md[1]
            close = md[2]
            identity =~ IDENTITY
            name = $1
            raise REXML::ParseException.new("DOCTYPE is missing a name") if name.nil?
            pub_sys = $2.nil? ? nil : $2.strip
            long_name = $4.nil? ? nil : $4.strip
            uri = $6.nil? ? nil : $6.strip
            args = [ :start_doctype, name, pub_sys, long_name, uri ]
            if close == ">"
              @document_status = :after_doctype
              @source.read if @source.buffer.size<2
              md = @source.match(/^\s*/um, true)
              @stack << [ :end_doctype ]
            else
              @document_status = :in_doctype
            end
            return args
          when /^\s+/
          else
            @document_status = :after_doctype
            @source.read if @source.buffer.size<2
            md = @source.match(/\s*/um, true)
            if @source.encoding == "UTF-8"
              @source.buffer.force_encoding(::Encoding::UTF_8)
            end
          end
        end
        if @document_status == :in_doctype
          md = @source.match(/\s*(.*?>)/um)
          case md[1]
          when SYSTEMENTITY
            match = @source.match( SYSTEMENTITY, true )[1]
            return [ :externalentity, match ]

          when ELEMENTDECL_START
            return [ :elementdecl, @source.match( ELEMENTDECL_PATTERN, true )[1] ]

          when ENTITY_START
            match = @source.match( ENTITYDECL, true ).to_a.compact
            match[0] = :entitydecl
            ref = false
            if match[1] == '%'
              ref = true
              match.delete_at 1
            end
            # Now we have to sort out what kind of entity reference this is
            if match[2] == 'SYSTEM'
              # External reference
              match[3] = match[3][1..-2] # PUBID
              match.delete_at(4) if match.size > 4 # Chop out NDATA decl
              # match is [ :entity, name, SYSTEM, pubid(, ndata)? ]
            elsif match[2] == 'PUBLIC'
              # External reference
              match[3] = match[3][1..-2] # PUBID
              match[4] = match[4][1..-2] # HREF
              # match is [ :entity, name, PUBLIC, pubid, href ]
            else
              match[2] = match[2][1..-2]
              match.pop if match.size == 4
              # match is [ :entity, name, value ]
            end
            match << '%' if ref
            return match
          when ATTLISTDECL_START
            md = @source.match( ATTLISTDECL_PATTERN, true )
            raise REXML::ParseException.new( "Bad ATTLIST declaration!", @source ) if md.nil?
            element = md[1]
            contents = md[0]

            pairs = {}
            values = md[0].scan( ATTDEF_RE )
            values.each do |attdef|
              unless attdef[3] == "#IMPLIED"
                attdef.compact!
                val = attdef[3]
                val = attdef[4] if val == "#FIXED "
                pairs[attdef[0]] = val
                if attdef[0] =~ /^xmlns:(.*)/
                  @nsstack[0] << $1
                end
              end
            end
            return [ :attlistdecl, element, pairs, contents ]
          when NOTATIONDECL_START
            md = nil
            if @source.match( PUBLIC )
              md = @source.match( PUBLIC, true )
              vals = [md[1],md[2],md[4],md[6]]
            elsif @source.match( SYSTEM )
              md = @source.match( SYSTEM, true )
              vals = [md[1],md[2],nil,md[4]]
            else
              raise REXML::ParseException.new( "error parsing notation: no matching pattern", @source )
            end
            return [ :notationdecl, *vals ]
          when CDATA_END
            @document_status = :after_doctype
            @source.match( CDATA_END, true )
            return [ :end_doctype ]
          end
        end
        begin
          if @source.buffer[0] == ?<
            if @source.buffer[1] == ?/
              @nsstack.shift
              last_tag = @tags.pop
              #md = @source.match_to_consume( '>', CLOSE_MATCH)
              md = @source.match( CLOSE_MATCH, true )
              raise REXML::ParseException.new( "Missing end tag for "+
                "'#{last_tag}' (got \"#{md[1]}\")",
                @source) unless last_tag == md[1]
              return [ :end_element, last_tag ]
            elsif @source.buffer[1] == ?!
              md = @source.match(/\A(\s*[^>]*>)/um)
              #STDERR.puts "SOURCE BUFFER = #{source.buffer}, #{source.buffer.size}"
              raise REXML::ParseException.new("Malformed node", @source) unless md
              if md[0][2] == ?-
                md = @source.match( COMMENT_PATTERN, true )

                case md[1]
                when /--/, /-\z/
                  raise REXML::ParseException.new("Malformed comment", @source)
                end

                return [ :comment, md[1] ] if md
              else
                md = @source.match( CDATA_PATTERN, true )
                return [ :cdata, md[1] ] if md
              end
              raise REXML::ParseException.new( "Declarations can only occur "+
                "in the doctype declaration.", @source)
            elsif @source.buffer[1] == ??
              md = @source.match( INSTRUCTION_PATTERN, true )
              return [ :processing_instruction, md[1], md[2] ] if md
              raise REXML::ParseException.new( "Bad instruction declaration",
                @source)
            else
              # Get the next tag
              md = @source.match(TAG_MATCH, true)
              unless md
                # Check for missing attribute quotes
                raise REXML::ParseException.new("missing attribute quote", @source) if @source.match(MISSING_ATTRIBUTE_QUOTES )
                raise REXML::ParseException.new("malformed XML: missing tag start", @source)
              end
              attributes = {}
              prefixes = Set.new
              prefixes << md[2] if md[2]
              @nsstack.unshift(curr_ns=Set.new)
              if md[4].size > 0
                attrs = md[4].scan( ATTRIBUTE_PATTERN )
                raise REXML::ParseException.new( "error parsing attributes: [#{attrs.join ', '}], excess = \"#$'\"", @source) if $' and $'.strip.size > 0
                attrs.each do |attr_name, prefix, local_part, quote, value|
                  if prefix == "xmlns"
                    if local_part == "xml"
                      if value != "http://www.w3.org/XML/1998/namespace"
                        msg = "The 'xml' prefix must not be bound to any other namespace "+
                        "(http://www.w3.org/TR/REC-xml-names/#ns-decl)"
                        raise REXML::ParseException.new( msg, @source, self )
                      end
                    elsif local_part == "xmlns"
                      msg = "The 'xmlns' prefix must not be declared "+
                      "(http://www.w3.org/TR/REC-xml-names/#ns-decl)"
                      raise REXML::ParseException.new( msg, @source, self)
                    end
                    curr_ns << local_part
                  elsif prefix
                    prefixes << prefix unless prefix == "xml"
                  end

                  if attributes.has_key?(attr_name)
                    msg = "Duplicate attribute #{attr_name.inspect}"
                    raise REXML::ParseException.new(msg, @source, self)
                  end

                  attributes[attr_name] = value
                end
              end

              # Verify that all of the prefixes have been defined
              for prefix in prefixes
                unless @nsstack.find{|k| k.member?(prefix)}
                  raise UndefinedNamespaceException.new(prefix,@source,self)
                end
              end

              if md[6]
                @closed = md[1]
                @nsstack.shift
              else
                @tags.push( md[1] )
              end
              return [ :start_element, md[1], attributes ]
            end
          else
            md = @source.match( TEXT_PATTERN, true )
            if md[0].length == 0
              @source.match( /(\s+)/, true )
            end
            #STDERR.puts "GOT #{md[1].inspect}" unless md[0].length == 0
            #return [ :text, "" ] if md[0].length == 0
            # unnormalized = Text::unnormalize( md[1], self )
            # return PullEvent.new( :text, md[1], unnormalized )
            return [ :text, md[1] ]
          end
        rescue REXML::UndefinedNamespaceException
          raise
        rescue REXML::ParseException
          raise
        rescue Exception, NameError => error
          raise REXML::ParseException.new( "Exception parsing",
            @source, self, (error ? error : $!) )
        end
        return [ :dummy ]
      end
      private :pull_event

      def entity( reference, entities )
        value = nil
        value = entities[ reference ] if entities
        if not value
          value = DEFAULT_ENTITIES[ reference ]
          value = value[2] if value
        end
        unnormalize( value, entities ) if value
      end

      # Escapes all possible entities
      def normalize( input, entities=nil, entity_filter=nil )
        copy = input.clone
        # Doing it like this rather than in a loop improves the speed
        copy.gsub!( EREFERENCE, '&amp;' )
        entities.each do |key, value|
          copy.gsub!( value, "&#{key};" ) unless entity_filter and
                                      entity_filter.include?(entity)
        end if entities
        copy.gsub!( EREFERENCE, '&amp;' )
        DEFAULT_ENTITIES.each do |key, value|
          copy.gsub!( value[3], value[1] )
        end
        copy
      end

      # Unescapes all possible entities
      def unnormalize( string, entities=nil, filter=nil )
        rv = string.clone
        rv.gsub!( /\r\n?/, "\n" )
        matches = rv.scan( REFERENCE_RE )
        return rv if matches.size == 0
        rv.gsub!( /&#0*((?:\d+)|(?:x[a-fA-F0-9]+));/ ) {
          m=$1
          m = "0#{m}" if m[0] == ?x
          [Integer(m)].pack('U*')
        }
        matches.collect!{|x|x[0]}.compact!
        if matches.size > 0
          matches.each do |entity_reference|
            unless filter and filter.include?(entity_reference)
              entity_value = entity( entity_reference, entities )
              if entity_value
                re = /&#{entity_reference};/
                rv.gsub!( re, entity_value )
              else
                er = DEFAULT_ENTITIES[entity_reference]
                rv.gsub!( er[0], er[2] ) if er
              end
            end
          end
          rv.gsub!( /&amp;/, '&' )
        end
        rv
      end
    end
  end
end

=begin
  case event[0]
  when :start_element
  when :text
  when :end_element
  when :processing_instruction
  when :cdata
  when :comment
  when :xmldecl
  when :start_doctype
  when :end_doctype
  when :externalentity
  when :elementdecl
  when :entity
  when :attlistdecl
  when :notationdecl
  when :end_doctype
  end
=end
