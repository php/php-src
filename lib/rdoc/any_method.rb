require 'rdoc/method_attr'
require 'rdoc/token_stream'

##
# AnyMethod is the base class for objects representing methods

class RDoc::AnyMethod < RDoc::MethodAttr

  MARSHAL_VERSION = 1 # :nodoc:

  ##
  # Don't rename \#initialize to \::new

  attr_accessor :dont_rename_initialize

  ##
  # The C function that implements this method (if it was defined in a C file)

  attr_accessor :c_function

  ##
  # Different ways to call this method

  attr_accessor :call_seq

  ##
  # Parameters for this method

  attr_accessor :params

  include RDoc::TokenStream

  ##
  # Creates a new AnyMethod with a token stream +text+ and +name+

  def initialize text, name
    super

    @c_function = nil
    @dont_rename_initialize = false
    @token_stream = nil
  end

  ##
  # Adds +an_alias+ as an alias for this method in +context+.

  def add_alias an_alias, context = nil
    method = self.class.new an_alias.text, an_alias.new_name

    method.record_location an_alias.file
    method.singleton = self.singleton
    method.params = self.params
    method.visibility = self.visibility
    method.comment = an_alias.comment
    method.is_alias_for = self
    @aliases << method
    context.add_method method if context
    method
  end

  ##
  # Prefix for +aref+ is 'method'.

  def aref_prefix
    'method'
  end

  ##
  # The call_seq or the param_seq with method name, if there is no call_seq.
  #
  # Use this for displaying a method's argument lists.

  def arglists
    if @call_seq then
      @call_seq
    elsif @params then
      "#{name}#{param_seq}"
    end
  end

  ##
  # Dumps this AnyMethod for use by ri.  See also #marshal_load

  def marshal_dump
    aliases = @aliases.map do |a|
      [a.name, parse(a.comment)]
    end

    [ MARSHAL_VERSION,
      @name,
      full_name,
      @singleton,
      @visibility,
      parse(@comment),
      @call_seq,
      @block_params,
      aliases,
      @params,
      @file.absolute_name,
    ]
  end

  ##
  # Loads this AnyMethod from +array+.  For a loaded AnyMethod the following
  # methods will return cached values:
  #
  # * #full_name
  # * #parent_name

  def marshal_load(array)
    @dont_rename_initialize = nil
    @is_alias_for           = nil
    @token_stream           = nil
    @aliases                = []

    version       = array[0]
    @name         = array[1]
    @full_name    = array[2]
    @singleton    = array[3]
    @visibility   = array[4]
    @comment      = array[5]
    @call_seq     = array[6]
    @block_params = array[7]

    array[8].each do |new_name, comment|
      add_alias RDoc::Alias.new(nil, @name, new_name, comment, @singleton)
    end

    @params       = array[9]

    @parent_name = if @full_name =~ /#/ then
                     $`
                   else
                     name = @full_name.split('::')
                     name.pop
                     name.join '::'
                   end

    @file = RDoc::TopLevel.new array[10] if version > 0
  end

  ##
  # Method name
  #
  # If the method has no assigned name, it extracts it from #call_seq.

  def name
    return @name if @name

    @name = @call_seq[/^.*?\.(\w+)/, 1] || @call_seq if @call_seq
  end

  ##
  # A list of this method's method and yield parameters.  +call-seq+ params
  # are preferred over parsed method and block params.

  def param_list
    if @call_seq then
      params = @call_seq.split("\n").last
      params = params.sub(/.*?\((.*)\)/, '\1')
      params = params.sub(/(\{|do)\s*\|([^|]*)\|.*/, ',\2')
    elsif @params then
      params = @params.sub(/\((.*)\)/, '\1')

      params << ",#{@block_params}" if @block_params
    elsif @block_params then
      params = @block_params
    else
      return []
    end

    params.gsub(/\s+/, '').split ','
  end

  ##
  # Pretty parameter list for this method.  If the method's parameters were
  # given by +call-seq+ it is preferred over the parsed values.

  def param_seq
    if @call_seq then
      params = @call_seq.split("\n").last
      params = params.sub(/[^( ]+/, '')
      params = params.sub(/(\|[^|]+\|)\s*\.\.\.\s*(end|\})/, '\1 \2')
    else
      params = @params.gsub(/\s*\#.*/, '')
      params = params.tr("\n", " ").squeeze(" ")
      params = "(#{params})" unless params[0] == ?(
    end

    if @block_params then
      # If this method has explicit block parameters, remove any explicit
      # &block
      params.sub!(/,?\s*&\w+/, '')

      block = @block_params.gsub(/\s*\#.*/, '')
      block = block.tr("\n", " ").squeeze(" ")
      if block[0] == ?(
        block.sub!(/^\(/, '').sub!(/\)/, '')
      end
      params << " { |#{block}| ... }"
    end

    params
  end

end

