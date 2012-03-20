require "rexml/parent"
require "rexml/namespace"
require "rexml/attribute"
require "rexml/cdata"
require "rexml/xpath"
require "rexml/parseexception"

module REXML
  # An implementation note about namespaces:
  # As we parse, when we find namespaces we put them in a hash and assign
  # them a unique ID.  We then convert the namespace prefix for the node
  # to the unique ID.  This makes namespace lookup much faster for the
  # cost of extra memory use.  We save the namespace prefix for the
  # context node and convert it back when we write it.
  @@namespaces = {}

  # Represents a tagged XML element.  Elements are characterized by
  # having children, attributes, and names, and can themselves be
  # children.
  class Element < Parent
    include Namespace

    UNDEFINED = "UNDEFINED";            # The default name

    # Mechanisms for accessing attributes and child elements of this
    # element.
    attr_reader :attributes, :elements
    # The context holds information about the processing environment, such as
    # whitespace handling.
    attr_accessor :context

    # Constructor
    # arg::
    #   if not supplied, will be set to the default value.
    #   If a String, the name of this object will be set to the argument.
    #   If an Element, the object will be shallowly cloned; name,
    #   attributes, and namespaces will be copied.  Children will +not+ be
    #   copied.
    # parent::
    #   if supplied, must be a Parent, and will be used as
    #   the parent of this object.
    # context::
    #   If supplied, must be a hash containing context items.  Context items
    #   include:
    # * <tt>:respect_whitespace</tt> the value of this is :+all+ or an array of
    #   strings being the names of the elements to respect
    #   whitespace for.  Defaults to :+all+.
    # * <tt>:compress_whitespace</tt> the value can be :+all+ or an array of
    #   strings being the names of the elements to ignore whitespace on.
    #   Overrides :+respect_whitespace+.
    # * <tt>:ignore_whitespace_nodes</tt> the value can be :+all+ or an array
    #   of strings being the names of the elements in which to ignore
    #   whitespace-only nodes.  If this is set, Text nodes which contain only
    #   whitespace will not be added to the document tree.
    # * <tt>:raw</tt> can be :+all+, or an array of strings being the names of
    #   the elements to process in raw mode.  In raw mode, special
    #   characters in text is not converted to or from entities.
    def initialize( arg = UNDEFINED, parent=nil, context=nil )
      super(parent)

      @elements = Elements.new(self)
      @attributes = Attributes.new(self)
      @context = context

      if arg.kind_of? String
        self.name = arg
      elsif arg.kind_of? Element
        self.name = arg.expanded_name
        arg.attributes.each_attribute{ |attribute|
          @attributes << Attribute.new( attribute )
        }
        @context = arg.context
      end
    end

    def inspect
      rv = "<#@expanded_name"

      @attributes.each_attribute do |attr|
        rv << " "
        attr.write( rv, 0 )
      end

      if children.size > 0
        rv << "> ... </>"
      else
        rv << "/>"
      end
    end


    # Creates a shallow copy of self.
    #   d = Document.new "<a><b/><b/><c><d/></c></a>"
    #   new_a = d.root.clone
    #   puts new_a  # => "<a/>"
    def clone
      self.class.new self
    end

    # Evaluates to the root node of the document that this element
    # belongs to. If this element doesn't belong to a document, but does
    # belong to another Element, the parent's root will be returned, until the
    # earliest ancestor is found.
    #
    # Note that this is not the same as the document element.
    # In the following example, <a> is the document element, and the root
    # node is the parent node of the document element.  You may ask yourself
    # why the root node is useful: consider the doctype and XML declaration,
    # and any processing instructions before the document element... they
    # are children of the root node, or siblings of the document element.
    # The only time this isn't true is when an Element is created that is
    # not part of any Document.  In this case, the ancestor that has no
    # parent acts as the root node.
    #  d = Document.new '<a><b><c/></b></a>'
    #  a = d[1] ; c = a[1][1]
    #  d.root_node == d   # TRUE
    #  a.root_node        # namely, d
    #  c.root_node        # again, d
    def root_node
      parent.nil? ? self : parent.root_node
    end

    def root
      return elements[1] if self.kind_of? Document
      return self if parent.kind_of? Document or parent.nil?
      return parent.root
    end

    # Evaluates to the document to which this element belongs, or nil if this
    # element doesn't belong to a document.
    def document
      rt = root
      rt.parent if rt
    end

    # Evaluates to +true+ if whitespace is respected for this element.  This
    # is the case if:
    # 1. Neither :+respect_whitespace+ nor :+compress_whitespace+ has any value
    # 2. The context has :+respect_whitespace+ set to :+all+ or
    #    an array containing the name of this element, and
    #    :+compress_whitespace+ isn't set to :+all+ or an array containing the
    #    name of this element.
    # The evaluation is tested against +expanded_name+, and so is namespace
    # sensitive.
    def whitespace
      @whitespace = nil
      if @context
        if @context[:respect_whitespace]
          @whitespace = (@context[:respect_whitespace] == :all or
                         @context[:respect_whitespace].include? expanded_name)
        end
        @whitespace = false if (@context[:compress_whitespace] and
                                (@context[:compress_whitespace] == :all or
                                 @context[:compress_whitespace].include? expanded_name)
                               )
      end
      @whitespace = true unless @whitespace == false
      @whitespace
    end

    def ignore_whitespace_nodes
      @ignore_whitespace_nodes = false
      if @context
        if @context[:ignore_whitespace_nodes]
          @ignore_whitespace_nodes =
            (@context[:ignore_whitespace_nodes] == :all or
             @context[:ignore_whitespace_nodes].include? expanded_name)
        end
      end
    end

    # Evaluates to +true+ if raw mode is set for this element.  This
    # is the case if the context has :+raw+ set to :+all+ or
    # an array containing the name of this element.
    #
    # The evaluation is tested against +expanded_name+, and so is namespace
    # sensitive.
    def raw
      @raw = (@context and @context[:raw] and
              (@context[:raw] == :all or
               @context[:raw].include? expanded_name))
               @raw
    end

    #once :whitespace, :raw, :ignore_whitespace_nodes

    #################################################
    # Namespaces                                    #
    #################################################

    # Evaluates to an +Array+ containing the prefixes (names) of all defined
    # namespaces at this context node.
    #  doc = Document.new("<a xmlns:x='1' xmlns:y='2'><b/><c xmlns:z='3'/></a>")
    #  doc.elements['//b'].prefixes # -> ['x', 'y']
    def prefixes
      prefixes = []
      prefixes = parent.prefixes if parent
      prefixes |= attributes.prefixes
      return prefixes
    end

    def namespaces
      namespaces = {}
      namespaces = parent.namespaces if parent
      namespaces = namespaces.merge( attributes.namespaces )
      return namespaces
    end

    # Evalutas to the URI for a prefix, or the empty string if no such
    # namespace is declared for this element. Evaluates recursively for
    # ancestors.  Returns the default namespace, if there is one.
    # prefix::
    #   the prefix to search for.  If not supplied, returns the default
    #   namespace if one exists
    # Returns::
    #   the namespace URI as a String, or nil if no such namespace
    #   exists.  If the namespace is undefined, returns an empty string
    #  doc = Document.new("<a xmlns='1' xmlns:y='2'><b/><c xmlns:z='3'/></a>")
    #  b = doc.elements['//b']
    #  b.namespace           # -> '1'
    #  b.namespace("y")      # -> '2'
    def namespace(prefix=nil)
      if prefix.nil?
        prefix = prefix()
      end
      if prefix == ''
        prefix = "xmlns"
      else
        prefix = "xmlns:#{prefix}" unless prefix[0,5] == 'xmlns'
      end
      ns = attributes[ prefix ]
      ns = parent.namespace(prefix) if ns.nil? and parent
      ns = '' if ns.nil? and prefix == 'xmlns'
      return ns
    end

    # Adds a namespace to this element.
    # prefix::
    #   the prefix string, or the namespace URI if +uri+ is not
    #   supplied
    # uri::
    #   the namespace URI.  May be nil, in which +prefix+ is used as
    #   the URI
    # Evaluates to: this Element
    #  a = Element.new("a")
    #  a.add_namespace("xmlns:foo", "bar" )
    #  a.add_namespace("foo", "bar")  # shorthand for previous line
    #  a.add_namespace("twiddle")
    #  puts a   #-> <a xmlns:foo='bar' xmlns='twiddle'/>
    def add_namespace( prefix, uri=nil )
      unless uri
        @attributes["xmlns"] = prefix
      else
        prefix = "xmlns:#{prefix}" unless prefix =~ /^xmlns:/
        @attributes[ prefix ] = uri
      end
      self
    end

    # Removes a namespace from this node.  This only works if the namespace is
    # actually declared in this node.  If no argument is passed, deletes the
    # default namespace.
    #
    # Evaluates to: this element
    #  doc = Document.new "<a xmlns:foo='bar' xmlns='twiddle'/>"
    #  doc.root.delete_namespace
    #  puts doc     # -> <a xmlns:foo='bar'/>
    #  doc.root.delete_namespace 'foo'
    #  puts doc     # -> <a/>
    def delete_namespace namespace="xmlns"
      namespace = "xmlns:#{namespace}" unless namespace == 'xmlns'
      attribute = attributes.get_attribute(namespace)
      attribute.remove unless attribute.nil?
      self
    end

    #################################################
    # Elements                                      #
    #################################################

    # Adds a child to this element, optionally setting attributes in
    # the element.
    # element::
    #   optional.  If Element, the element is added.
    #   Otherwise, a new Element is constructed with the argument (see
    #   Element.initialize).
    # attrs::
    #   If supplied, must be a Hash containing String name,value
    #   pairs, which will be used to set the attributes of the new Element.
    # Returns:: the Element that was added
    #  el = doc.add_element 'my-tag'
    #  el = doc.add_element 'my-tag', {'attr1'=>'val1', 'attr2'=>'val2'}
    #  el = Element.new 'my-tag'
    #  doc.add_element el
    def add_element element, attrs=nil
      raise "First argument must be either an element name, or an Element object" if element.nil?
      el = @elements.add(element)
      attrs.each do |key, value|
        el.attributes[key]=value
      end       if attrs.kind_of? Hash
      el
    end

    # Deletes a child element.
    # element::
    #   Must be an +Element+, +String+, or +Integer+.  If Element,
    #   the element is removed.  If String, the element is found (via XPath)
    #   and removed.  <em>This means that any parent can remove any
    #   descendant.<em>  If Integer, the Element indexed by that number will be
    #   removed.
    # Returns:: the element that was removed.
    #  doc.delete_element "/a/b/c[@id='4']"
    #  doc.delete_element doc.elements["//k"]
    #  doc.delete_element 1
    def delete_element element
      @elements.delete element
    end

    # Evaluates to +true+ if this element has at least one child Element
    #  doc = Document.new "<a><b/><c>Text</c></a>"
    #  doc.root.has_elements               # -> true
    #  doc.elements["/a/b"].has_elements   # -> false
    #  doc.elements["/a/c"].has_elements   # -> false
    def has_elements?
      !@elements.empty?
    end

    # Iterates through the child elements, yielding for each Element that
    # has a particular attribute set.
    # key::
    #   the name of the attribute to search for
    # value::
    #   the value of the attribute
    # max::
    #   (optional) causes this method to return after yielding
    #   for this number of matching children
    # name::
    #   (optional) if supplied, this is an XPath that filters
    #   the children to check.
    #
    #  doc = Document.new "<a><b @id='1'/><c @id='2'/><d @id='1'/><e/></a>"
    #  # Yields b, c, d
    #  doc.root.each_element_with_attribute( 'id' ) {|e| p e}
    #  # Yields b, d
    #  doc.root.each_element_with_attribute( 'id', '1' ) {|e| p e}
    #  # Yields b
    #  doc.root.each_element_with_attribute( 'id', '1', 1 ) {|e| p e}
    #  # Yields d
    #  doc.root.each_element_with_attribute( 'id', '1', 0, 'd' ) {|e| p e}
    def each_element_with_attribute( key, value=nil, max=0, name=nil, &block ) # :yields: Element
      each_with_something( proc {|child|
        if value.nil?
          child.attributes[key] != nil
        else
          child.attributes[key]==value
        end
      }, max, name, &block )
    end

    # Iterates through the children, yielding for each Element that
    # has a particular text set.
    # text::
    #   the text to search for.  If nil, or not supplied, will iterate
    #   over all +Element+ children that contain at least one +Text+ node.
    # max::
    #   (optional) causes this method to return after yielding
    #   for this number of matching children
    # name::
    #   (optional) if supplied, this is an XPath that filters
    #   the children to check.
    #
    #  doc = Document.new '<a><b>b</b><c>b</c><d>d</d><e/></a>'
    #  # Yields b, c, d
    #  doc.each_element_with_text {|e|p e}
    #  # Yields b, c
    #  doc.each_element_with_text('b'){|e|p e}
    #  # Yields b
    #  doc.each_element_with_text('b', 1){|e|p e}
    #  # Yields d
    #  doc.each_element_with_text(nil, 0, 'd'){|e|p e}
    def each_element_with_text( text=nil, max=0, name=nil, &block ) # :yields: Element
      each_with_something( proc {|child|
        if text.nil?
          child.has_text?
        else
          child.text == text
        end
      }, max, name, &block )
    end

    # Synonym for Element.elements.each
    def each_element( xpath=nil, &block ) # :yields: Element
      @elements.each( xpath, &block )
    end

    # Synonym for Element.to_a
    # This is a little slower than calling elements.each directly.
    # xpath:: any XPath by which to search for elements in the tree
    # Returns:: an array of Elements that match the supplied path
    def get_elements( xpath )
      @elements.to_a( xpath )
    end

    # Returns the next sibling that is an element, or nil if there is
    # no Element sibling after this one
    #  doc = Document.new '<a><b/>text<c/></a>'
    #  doc.root.elements['b'].next_element          #-> <c/>
    #  doc.root.elements['c'].next_element          #-> nil
    def next_element
      element = next_sibling
      element = element.next_sibling until element.nil? or element.kind_of? Element
      return element
    end

    # Returns the previous sibling that is an element, or nil if there is
    # no Element sibling prior to this one
    #  doc = Document.new '<a><b/>text<c/></a>'
    #  doc.root.elements['c'].previous_element          #-> <b/>
    #  doc.root.elements['b'].previous_element          #-> nil
    def previous_element
      element = previous_sibling
      element = element.previous_sibling until element.nil? or element.kind_of? Element
      return element
    end


    #################################################
    # Text                                          #
    #################################################

    # Evaluates to +true+ if this element has at least one Text child
    def has_text?
      not text().nil?
    end

    # A convenience method which returns the String value of the _first_
    # child text element, if one exists, and +nil+ otherwise.
    #
    # <em>Note that an element may have multiple Text elements, perhaps
    # separated by other children</em>.  Be aware that this method only returns
    # the first Text node.
    #
    # This method returns the +value+ of the first text child node, which
    # ignores the +raw+ setting, so always returns normalized text. See
    # the Text::value documentation.
    #
    #  doc = Document.new "<p>some text <b>this is bold!</b> more text</p>"
    #  # The element 'p' has two text elements, "some text " and " more text".
    #  doc.root.text              #-> "some text "
    def text( path = nil )
      rv = get_text(path)
      return rv.value unless rv.nil?
      nil
    end

    # Returns the first child Text node, if any, or +nil+ otherwise.
    # This method returns the actual +Text+ node, rather than the String content.
    #  doc = Document.new "<p>some text <b>this is bold!</b> more text</p>"
    #  # The element 'p' has two text elements, "some text " and " more text".
    #  doc.root.get_text.value            #-> "some text "
    def get_text path = nil
      rv = nil
      if path
        element = @elements[ path ]
        rv = element.get_text unless element.nil?
      else
        rv = @children.find { |node| node.kind_of? Text }
      end
      return rv
    end

    # Sets the first Text child of this object.  See text() for a
    # discussion about Text children.
    #
    # If a Text child already exists, the child is replaced by this
    # content.  This means that Text content can be deleted by calling
    # this method with a nil argument.  In this case, the next Text
    # child becomes the first Text child.  In no case is the order of
    # any siblings disturbed.
    # text::
    #   If a String, a new Text child is created and added to
    #   this Element as the first Text child.  If Text, the text is set
    #   as the first Child element.  If nil, then any existing first Text
    #   child is removed.
    # Returns:: this Element.
    #  doc = Document.new '<a><b/></a>'
    #  doc.root.text = 'Sean'      #-> '<a><b/>Sean</a>'
    #  doc.root.text = 'Elliott'   #-> '<a><b/>Elliott</a>'
    #  doc.root.add_element 'c'    #-> '<a><b/>Elliott<c/></a>'
    #  doc.root.text = 'Russell'   #-> '<a><b/>Russell<c/></a>'
    #  doc.root.text = nil         #-> '<a><b/><c/></a>'
    def text=( text )
      if text.kind_of? String
        text = Text.new( text, whitespace(), nil, raw() )
      elsif !text.nil? and !text.kind_of? Text
        text = Text.new( text.to_s, whitespace(), nil, raw() )
      end
      old_text = get_text
      if text.nil?
        old_text.remove unless old_text.nil?
      else
        if old_text.nil?
          self << text
        else
          old_text.replace_with( text )
        end
      end
      return self
    end

    # A helper method to add a Text child.  Actual Text instances can
    # be added with regular Parent methods, such as add() and <<()
    # text::
    #   if a String, a new Text instance is created and added
    #   to the parent.  If Text, the object is added directly.
    # Returns:: this Element
    #  e = Element.new('a')          #-> <e/>
    #  e.add_text 'foo'              #-> <e>foo</e>
    #  e.add_text Text.new(' bar')    #-> <e>foo bar</e>
    # Note that at the end of this example, the branch has <b>3</b> nodes; the 'e'
    # element and <b>2</b> Text node children.
    def add_text( text )
      if text.kind_of? String
        if @children[-1].kind_of? Text
          @children[-1] << text
          return
        end
        text = Text.new( text, whitespace(), nil, raw() )
      end
      self << text unless text.nil?
      return self
    end

    def node_type
      :element
    end

    def xpath
      path_elements = []
      cur = self
      path_elements << __to_xpath_helper( self )
      while cur.parent
        cur = cur.parent
        path_elements << __to_xpath_helper( cur )
      end
      return path_elements.reverse.join( "/" )
    end

    #################################################
    # Attributes                                    #
    #################################################

    def attribute( name, namespace=nil )
      prefix = nil
      if namespaces.respond_to? :key
        prefix = namespaces.key(namespace) if namespace
      else
        prefix = namespaces.index(namespace) if namespace
      end
      prefix = nil if prefix == 'xmlns'

      ret_val =
        attributes.get_attribute( "#{prefix ? prefix + ':' : ''}#{name}" )

      return ret_val unless ret_val.nil?
      return nil if prefix.nil?

      # now check that prefix'es namespace is not the same as the
      # default namespace
      return nil unless ( namespaces[ prefix ] == namespaces[ 'xmlns' ] )

      attributes.get_attribute( name )

    end

    # Evaluates to +true+ if this element has any attributes set, false
    # otherwise.
    def has_attributes?
      return !@attributes.empty?
    end

    # Adds an attribute to this element, overwriting any existing attribute
    # by the same name.
    # key::
    #   can be either an Attribute or a String.  If an Attribute,
    #   the attribute is added to the list of Element attributes.  If String,
    #   the argument is used as the name of the new attribute, and the value
    #   parameter must be supplied.
    # value::
    #   Required if +key+ is a String, and ignored if the first argument is
    #   an Attribute.  This is a String, and is used as the value
    #   of the new Attribute.  This should be the unnormalized value of the
    #   attribute (without entities).
    # Returns:: the Attribute added
    #  e = Element.new 'e'
    #  e.add_attribute( 'a', 'b' )               #-> <e a='b'/>
    #  e.add_attribute( 'x:a', 'c' )             #-> <e a='b' x:a='c'/>
    #  e.add_attribute Attribute.new('b', 'd')   #-> <e a='b' x:a='c' b='d'/>
    def add_attribute( key, value=nil )
      if key.kind_of? Attribute
        @attributes << key
      else
        @attributes[key] = value
      end
    end

    # Add multiple attributes to this element.
    # hash:: is either a hash, or array of arrays
    #  el.add_attributes( {"name1"=>"value1", "name2"=>"value2"} )
    #  el.add_attributes( [ ["name1","value1"], ["name2"=>"value2"] ] )
    def add_attributes hash
      if hash.kind_of? Hash
        hash.each_pair {|key, value| @attributes[key] = value }
      elsif hash.kind_of? Array
        hash.each { |value| @attributes[ value[0] ] = value[1] }
      end
    end

    # Removes an attribute
    # key::
    #   either an Attribute or a String.  In either case, the
    #   attribute is found by matching the attribute name to the argument,
    #   and then removed.  If no attribute is found, no action is taken.
    # Returns::
    #   the attribute removed, or nil if this Element did not contain
    #   a matching attribute
    #  e = Element.new('E')
    #  e.add_attribute( 'name', 'Sean' )             #-> <E name='Sean'/>
    #  r = e.add_attribute( 'sur:name', 'Russell' )  #-> <E name='Sean' sur:name='Russell'/>
    #  e.delete_attribute( 'name' )                  #-> <E sur:name='Russell'/>
    #  e.delete_attribute( r )                       #-> <E/>
    def delete_attribute(key)
      attr = @attributes.get_attribute(key)
      attr.remove unless attr.nil?
    end

    #################################################
    # Other Utilities                               #
    #################################################

    # Get an array of all CData children.
    # IMMUTABLE
    def cdatas
      find_all { |child| child.kind_of? CData }.freeze
    end

    # Get an array of all Comment children.
    # IMMUTABLE
    def comments
      find_all { |child| child.kind_of? Comment }.freeze
    end

    # Get an array of all Instruction children.
    # IMMUTABLE
    def instructions
      find_all { |child| child.kind_of? Instruction }.freeze
    end

    # Get an array of all Text children.
    # IMMUTABLE
    def texts
      find_all { |child| child.kind_of? Text }.freeze
    end

    # == DEPRECATED
    # See REXML::Formatters
    #
    # Writes out this element, and recursively, all children.
    # output::
    #     output an object which supports '<< string'; this is where the
    #   document will be written.
    # indent::
    #   An integer.  If -1, no indenting will be used; otherwise, the
    #   indentation will be this number of spaces, and children will be
    #   indented an additional amount.  Defaults to -1
    # transitive::
    #   If transitive is true and indent is >= 0, then the output will be
    #   pretty-printed in such a way that the added whitespace does not affect
    #   the parse tree of the document
    # ie_hack::
    #   Internet Explorer is the worst piece of crap to have ever been
    #   written, with the possible exception of Windows itself.  Since IE is
    #   unable to parse proper XML, we have to provide a hack to generate XML
    #   that IE's limited abilities can handle.  This hack inserts a space
    #   before the /> on empty tags.  Defaults to false
    #
    #  out = ''
    #  doc.write( out )     #-> doc is written to the string 'out'
    #  doc.write( $stdout ) #-> doc written to the console
    def write(output=$stdout, indent=-1, transitive=false, ie_hack=false)
      Kernel.warn("#{self.class.name}.write is deprecated.  See REXML::Formatters")
      formatter = if indent > -1
          if transitive
            require "rexml/formatters/transitive"
            REXML::Formatters::Transitive.new( indent, ie_hack )
          else
            REXML::Formatters::Pretty.new( indent, ie_hack )
          end
        else
          REXML::Formatters::Default.new( ie_hack )
        end
      formatter.write( self, output )
    end


    private
    def __to_xpath_helper node
      rv = node.expanded_name.clone
      if node.parent
        results = node.parent.find_all {|n|
          n.kind_of?(REXML::Element) and n.expanded_name == node.expanded_name
        }
        if results.length > 1
          idx = results.index( node )
          rv << "[#{idx+1}]"
        end
      end
      rv
    end

    # A private helper method
    def each_with_something( test, max=0, name=nil )
      num = 0
      @elements.each( name ){ |child|
        yield child if test.call(child) and num += 1
        return if max>0 and num == max
      }
    end
  end

  ########################################################################
  # ELEMENTS                                                             #
  ########################################################################

  # A class which provides filtering of children for Elements, and
  # XPath search support.  You are expected to only encounter this class as
  # the <tt>element.elements</tt> object.  Therefore, you are
  # _not_ expected to instantiate this yourself.
  class Elements
    include Enumerable
    # Constructor
    # parent:: the parent Element
    def initialize parent
      @element = parent
    end

    # Fetches a child element.  Filters only Element children, regardless of
    # the XPath match.
    # index::
    #   the search parameter.  This is either an Integer, which
    #   will be used to find the index'th child Element, or an XPath,
    #   which will be used to search for the Element.  <em>Because
    #   of the nature of XPath searches, any element in the connected XML
    #   document can be fetched through any other element.</em>  <b>The
    #   Integer index is 1-based, not 0-based.</b>  This means that the first
    #   child element is at index 1, not 0, and the +n+th element is at index
    #   +n+, not <tt>n-1</tt>.  This is because XPath indexes element children
    #   starting from 1, not 0, and the indexes should be the same.
    # name::
    #   optional, and only used in the first argument is an
    #   Integer.  In that case, the index'th child Element that has the
    #   supplied name will be returned.  Note again that the indexes start at 1.
    # Returns:: the first matching Element, or nil if no child matched
    #  doc = Document.new '<a><b/><c id="1"/><c id="2"/><d/></a>'
    #  doc.root.elements[1]       #-> <b/>
    #  doc.root.elements['c']     #-> <c id="1"/>
    #  doc.root.elements[2,'c']   #-> <c id="2"/>
    def []( index, name=nil)
      if index.kind_of? Integer
        raise "index (#{index}) must be >= 1" if index < 1
        name = literalize(name) if name
        num = 0
        @element.find { |child|
          child.kind_of? Element and
          (name.nil? ? true : child.has_name?( name )) and
          (num += 1) == index
        }
      else
        return XPath::first( @element, index )
        #{ |element|
        #       return element if element.kind_of? Element
        #}
        #return nil
      end
    end

    # Sets an element, replacing any previous matching element.  If no
    # existing element is found ,the element is added.
    # index:: Used to find a matching element to replace.  See []().
    # element::
    #   The element to replace the existing element with
    #   the previous element
    # Returns:: nil if no previous element was found.
    #
    #  doc = Document.new '<a/>'
    #  doc.root.elements[10] = Element.new('b')    #-> <a><b/></a>
    #  doc.root.elements[1]                        #-> <b/>
    #  doc.root.elements[1] = Element.new('c')     #-> <a><c/></a>
    #  doc.root.elements['c'] = Element.new('d')   #-> <a><d/></a>
    def []=( index, element )
      previous = self[index]
      if previous.nil?
        @element.add element
      else
        previous.replace_with element
      end
      return previous
    end

    # Returns +true+ if there are no +Element+ children, +false+ otherwise
    def empty?
      @element.find{ |child| child.kind_of? Element}.nil?
    end

    # Returns the index of the supplied child (starting at 1), or -1 if
    # the element is not a child
    # element:: an +Element+ child
    def index element
      rv = 0
      found = @element.find do |child|
        child.kind_of? Element and
        (rv += 1) and
        child == element
      end
      return rv if found == element
      return -1
    end

    # Deletes a child Element
    # element::
    #   Either an Element, which is removed directly; an
    #   xpath, where the first matching child is removed; or an Integer,
    #   where the n'th Element is removed.
    # Returns:: the removed child
    #  doc = Document.new '<a><b/><c/><c id="1"/></a>'
    #  b = doc.root.elements[1]
    #  doc.root.elements.delete b           #-> <a><c/><c id="1"/></a>
    #  doc.elements.delete("a/c[@id='1']")  #-> <a><c/></a>
    #  doc.root.elements.delete 1           #-> <a/>
    def delete element
      if element.kind_of? Element
        @element.delete element
      else
        el = self[element]
        el.remove if el
      end
    end

    # Removes multiple elements.  Filters for Element children, regardless of
    # XPath matching.
    # xpath:: all elements matching this String path are removed.
    # Returns:: an Array of Elements that have been removed
    #  doc = Document.new '<a><c/><c/><c/><c/></a>'
    #  deleted = doc.elements.delete_all 'a/c' #-> [<c/>, <c/>, <c/>, <c/>]
    def delete_all( xpath )
      rv = []
      XPath::each( @element, xpath) {|element|
        rv << element if element.kind_of? Element
      }
      rv.each do |element|
        @element.delete element
        element.remove
      end
      return rv
    end

    # Adds an element
    # element::
    #   if supplied, is either an Element, String, or
    #   Source (see Element.initialize).  If not supplied or nil, a
    #   new, default Element will be constructed
    # Returns:: the added Element
    #  a = Element.new('a')
    #  a.elements.add(Element.new('b'))  #-> <a><b/></a>
    #  a.elements.add('c')               #-> <a><b/><c/></a>
    def add element=nil
      if element.nil?
        Element.new("", self, @element.context)
      elsif not element.kind_of?(Element)
        Element.new(element, self, @element.context)
      else
        @element << element
        element.context = @element.context
        element
      end
    end

    alias :<< :add

    # Iterates through all of the child Elements, optionally filtering
    # them by a given XPath
    # xpath::
    #   optional.  If supplied, this is a String XPath, and is used to
    #   filter the children, so that only matching children are yielded.  Note
    #   that XPaths are automatically filtered for Elements, so that
    #   non-Element children will not be yielded
    #  doc = Document.new '<a><b/><c/><d/>sean<b/><c/><d/></a>'
    #  doc.root.each {|e|p e}       #-> Yields b, c, d, b, c, d elements
    #  doc.root.each('b') {|e|p e}  #-> Yields b, b elements
    #  doc.root.each('child::node()')  {|e|p e}
    #  #-> Yields <b/>, <c/>, <d/>, <b/>, <c/>, <d/>
    #  XPath.each(doc.root, 'child::node()', &block)
    #  #-> Yields <b/>, <c/>, <d/>, sean, <b/>, <c/>, <d/>
    def each( xpath=nil )
      XPath::each( @element, xpath ) {|e| yield e if e.kind_of? Element }
    end

    def collect( xpath=nil )
      collection = []
      XPath::each( @element, xpath ) {|e|
        collection << yield(e)  if e.kind_of?(Element)
      }
      collection
    end

    def inject( xpath=nil, initial=nil )
      first = true
      XPath::each( @element, xpath ) {|e|
        if (e.kind_of? Element)
          if (first and initial == nil)
            initial = e
            first = false
          else
            initial = yield( initial, e ) if e.kind_of? Element
          end
        end
      }
      initial
    end

    # Returns the number of +Element+ children of the parent object.
    #  doc = Document.new '<a>sean<b/>elliott<b/>russell<b/></a>'
    #  doc.root.size            #-> 6, 3 element and 3 text nodes
    #  doc.root.elements.size   #-> 3
    def size
      count = 0
      @element.each {|child| count+=1 if child.kind_of? Element }
      count
    end

    # Returns an Array of Element children.  An XPath may be supplied to
    # filter the children.  Only Element children are returned, even if the
    # supplied XPath matches non-Element children.
    #  doc = Document.new '<a>sean<b/>elliott<c/></a>'
    #  doc.root.elements.to_a                  #-> [ <b/>, <c/> ]
    #  doc.root.elements.to_a("child::node()") #-> [ <b/>, <c/> ]
    #  XPath.match(doc.root, "child::node()")  #-> [ sean, <b/>, elliott, <c/> ]
    def to_a( xpath=nil )
      rv = XPath.match( @element, xpath )
      return rv.find_all{|e| e.kind_of? Element} if xpath
      rv
    end

    private
    # Private helper class.  Removes quotes from quoted strings
    def literalize name
      name = name[1..-2] if name[0] == ?' or name[0] == ?"               #'
      name
    end
  end

  ########################################################################
  # ATTRIBUTES                                                           #
  ########################################################################

  # A class that defines the set of Attributes of an Element and provides
  # operations for accessing elements in that set.
  class Attributes < Hash
    # Constructor
    # element:: the Element of which this is an Attribute
    def initialize element
      @element = element
    end

    # Fetches an attribute value.  If you want to get the Attribute itself,
    # use get_attribute()
    # name:: an XPath attribute name.  Namespaces are relevant here.
    # Returns::
    #   the String value of the matching attribute, or +nil+ if no
    #   matching attribute was found.  This is the unnormalized value
    #   (with entities expanded).
    #
    #  doc = Document.new "<a foo:att='1' bar:att='2' att='&lt;'/>"
    #  doc.root.attributes['att']         #-> '<'
    #  doc.root.attributes['bar:att']     #-> '2'
    def [](name)
      attr = get_attribute(name)
      return attr.value unless attr.nil?
      return nil
    end

    def to_a
      values.flatten
    end

    # Returns the number of attributes the owning Element contains.
    #  doc = Document "<a x='1' y='2' foo:x='3'/>"
    #  doc.root.attributes.length        #-> 3
    def length
      c = 0
      each_attribute { c+=1 }
      c
    end
    alias :size :length

    # Iterates over the attributes of an Element.  Yields actual Attribute
    # nodes, not String values.
    #
    #  doc = Document.new '<a x="1" y="2"/>'
    #  doc.root.attributes.each_attribute {|attr|
    #    p attr.expanded_name+" => "+attr.value
    #  }
    def each_attribute # :yields: attribute
      each_value do |val|
        if val.kind_of? Attribute
          yield val
        else
          val.each_value { |atr| yield atr }
        end
      end
    end

    # Iterates over each attribute of an Element, yielding the expanded name
    # and value as a pair of Strings.
    #
    #  doc = Document.new '<a x="1" y="2"/>'
    #  doc.root.attributes.each {|name, value| p name+" => "+value }
    def each
      each_attribute do |attr|
        yield [attr.expanded_name, attr.value]
      end
    end

    # Fetches an attribute
    # name::
    #   the name by which to search for the attribute.  Can be a
    #   <tt>prefix:name</tt> namespace name.
    # Returns:: The first matching attribute, or nil if there was none.  This
    # value is an Attribute node, not the String value of the attribute.
    #  doc = Document.new '<a x:foo="1" foo="2" bar="3"/>'
    #  doc.root.attributes.get_attribute("foo").value    #-> "2"
    #  doc.root.attributes.get_attribute("x:foo").value  #-> "1"
    def get_attribute( name )
      attr = fetch( name, nil )
      if attr.nil?
        return nil if name.nil?
        # Look for prefix
        name =~ Namespace::NAMESPLIT
        prefix, n = $1, $2
        if prefix
          attr = fetch( n, nil )
          # check prefix
          if attr == nil
          elsif attr.kind_of? Attribute
            return attr if prefix == attr.prefix
          else
            attr = attr[ prefix ]
            return attr
          end
        end
        element_document = @element.document
        if element_document and element_document.doctype
          expn = @element.expanded_name
          expn = element_document.doctype.name if expn.size == 0
          attr_val = element_document.doctype.attribute_of(expn, name)
          return Attribute.new( name, attr_val ) if attr_val
        end
        return nil
      end
      if attr.kind_of? Hash
        attr = attr[ @element.prefix ]
      end
      return attr
    end

    # Sets an attribute, overwriting any existing attribute value by the
    # same name.  Namespace is significant.
    # name:: the name of the attribute
    # value::
    #   (optional) If supplied, the value of the attribute.  If
    #   nil, any existing matching attribute is deleted.
    # Returns::
    #   Owning element
    #  doc = Document.new "<a x:foo='1' foo='3'/>"
    #  doc.root.attributes['y:foo'] = '2'
    #  doc.root.attributes['foo'] = '4'
    #  doc.root.attributes['x:foo'] = nil
    def []=( name, value )
      if value.nil?             # Delete the named attribute
        attr = get_attribute(name)
        delete attr
        return
      end

      unless value.kind_of? Attribute
        if @element.document and @element.document.doctype
          value = Text::normalize( value, @element.document.doctype )
        else
          value = Text::normalize( value, nil )
        end
        value = Attribute.new(name, value)
      end
      value.element = @element
      old_attr = fetch(value.name, nil)
      if old_attr.nil?
        store(value.name, value)
      elsif old_attr.kind_of? Hash
        old_attr[value.prefix] = value
      elsif old_attr.prefix != value.prefix
        # Check for conflicting namespaces
        raise ParseException.new(
          "Namespace conflict in adding attribute \"#{value.name}\": "+
          "Prefix \"#{old_attr.prefix}\" = "+
          "\"#{@element.namespace(old_attr.prefix)}\" and prefix "+
          "\"#{value.prefix}\" = \"#{@element.namespace(value.prefix)}\"") if
          value.prefix != "xmlns" and old_attr.prefix != "xmlns" and
          @element.namespace( old_attr.prefix ) ==
            @element.namespace( value.prefix )
          store value.name, { old_attr.prefix   => old_attr,
            value.prefix                => value }
      else
        store value.name, value
      end
      return @element
    end

    # Returns an array of Strings containing all of the prefixes declared
    # by this set of # attributes.  The array does not include the default
    # namespace declaration, if one exists.
    #  doc = Document.new("<a xmlns='foo' xmlns:x='bar' xmlns:y='twee' "+
    #        "z='glorp' p:k='gru'/>")
    #  prefixes = doc.root.attributes.prefixes    #-> ['x', 'y']
    def prefixes
      ns = []
      each_attribute do |attribute|
        ns << attribute.name if attribute.prefix == 'xmlns'
      end
      if @element.document and @element.document.doctype
        expn = @element.expanded_name
        expn = @element.document.doctype.name if expn.size == 0
        @element.document.doctype.attributes_of(expn).each {
          |attribute|
          ns << attribute.name if attribute.prefix == 'xmlns'
        }
      end
      ns
    end

    def namespaces
      namespaces = {}
      each_attribute do |attribute|
        namespaces[attribute.name] = attribute.value if attribute.prefix == 'xmlns' or attribute.name == 'xmlns'
      end
      if @element.document and @element.document.doctype
        expn = @element.expanded_name
        expn = @element.document.doctype.name if expn.size == 0
        @element.document.doctype.attributes_of(expn).each {
          |attribute|
          namespaces[attribute.name] = attribute.value if attribute.prefix == 'xmlns' or attribute.name == 'xmlns'
        }
      end
      namespaces
    end

    # Removes an attribute
    # attribute::
    #   either a String, which is the name of the attribute to remove --
    #   namespaces are significant here -- or the attribute to remove.
    # Returns:: the owning element
    #  doc = Document.new "<a y:foo='0' x:foo='1' foo='3' z:foo='4'/>"
    #  doc.root.attributes.delete 'foo'   #-> <a y:foo='0' x:foo='1' z:foo='4'/>"
    #  doc.root.attributes.delete 'x:foo' #-> <a y:foo='0' z:foo='4'/>"
    #  attr = doc.root.attributes.get_attribute('y:foo')
    #  doc.root.attributes.delete attr    #-> <a z:foo='4'/>"
    def delete( attribute )
      name = nil
      prefix = nil
      if attribute.kind_of? Attribute
        name = attribute.name
        prefix = attribute.prefix
      else
        attribute =~ Namespace::NAMESPLIT
        prefix, name = $1, $2
        prefix = '' unless prefix
      end
      old = fetch(name, nil)
      attr = nil
      if old.kind_of? Hash # the supplied attribute is one of many
        attr = old.delete(prefix)
        if old.size == 1
          repl = nil
          old.each_value{|v| repl = v}
          store name, repl
        end
      elsif old.nil?
        return @element
      else # the supplied attribute is a top-level one
        attr = old
        super(name)
      end
      @element
    end

    # Adds an attribute, overriding any existing attribute by the
    # same name.  Namespaces are significant.
    # attribute:: An Attribute
    def add( attribute )
      self[attribute.name] = attribute
    end

    alias :<< :add

    # Deletes all attributes matching a name.  Namespaces are significant.
    # name::
    #   A String; all attributes that match this path will be removed
    # Returns:: an Array of the Attributes that were removed
    def delete_all( name )
      rv = []
      each_attribute { |attribute|
        rv << attribute if attribute.expanded_name == name
      }
      rv.each{ |attr| attr.remove }
      return rv
    end

    # The +get_attribute_ns+ method retrieves a method by its namespace
    # and name. Thus it is possible to reliably identify an attribute
    # even if an XML processor has changed the prefix.
    #
    # Method contributed by Henrik Martensson
    def get_attribute_ns(namespace, name)
      result = nil
      each_attribute() { |attribute|
        if name == attribute.name &&
          namespace == attribute.namespace() &&
          ( !namespace.empty? || !attribute.fully_expanded_name.index(':') )
          # foo will match xmlns:foo, but only if foo isn't also an attribute
          result = attribute if !result or !namespace.empty? or
                                !attribute.fully_expanded_name.index(':')
        end
      }
      result
    end
  end
end
