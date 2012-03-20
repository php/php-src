# = uri/ldap.rb
#
# Author::
#  Takaaki Tateishi <ttate@jaist.ac.jp>
#  Akira Yamada <akira@ruby-lang.org>
# License::
#   URI::LDAP is copyrighted free software by Takaaki Tateishi and Akira Yamada.
#   You can redistribute it and/or modify it under the same term as Ruby.
# Revision:: $Id$
#
# See URI for general documentation
#

require 'uri/generic'

module URI

  #
  # LDAP URI SCHEMA (described in RFC2255)
  # ldap://<host>/<dn>[?<attrs>[?<scope>[?<filter>[?<extensions>]]]]
  #
  class LDAP < Generic

    # A Default port of 389 for URI::LDAP
    DEFAULT_PORT = 389

    # An Array of the available components for URI::LDAP
    COMPONENT = [
      :scheme,
      :host, :port,
      :dn,
      :attributes,
      :scope,
      :filter,
      :extensions,
    ].freeze

    # Scopes available for the starting point.
    #
    # * SCOPE_BASE - the Base DN
    # * SCOPE_ONE  - one level under the Base DN, not including the base DN and
    #                not including any entries under this.
    # * SCOPE_SUB  - subtress, all entries at all levels
    #
    SCOPE = [
      SCOPE_ONE = 'one',
      SCOPE_SUB = 'sub',
      SCOPE_BASE = 'base',
    ].freeze

    #
    # == Description
    #
    # Create a new URI::LDAP object from components, with syntax checking.
    #
    # The components accepted are host, port, dn, attributes,
    # scope, filter, and extensions.
    #
    # The components should be provided either as an Array, or as a Hash
    # with keys formed by preceding the component names with a colon.
    #
    # If an Array is used, the components must be passed in the order
    # [host, port, dn, attributes, scope, filter, extensions].
    #
    # Example:
    #
    #     newuri = URI::LDAP.build({:host => 'ldap.example.com',
    #       :dn> => '/dc=example'})
    #
    #     newuri = URI::LDAP.build(["ldap.example.com", nil,
    #       "/dc=example;dc=com", "query", nil, nil, nil])
    #
    def self.build(args)
      tmp = Util::make_components_hash(self, args)

      if tmp[:dn]
        tmp[:path] = tmp[:dn]
      end

      query = []
      [:extensions, :filter, :scope, :attributes].collect do |x|
        next if !tmp[x] && query.size == 0
        query.unshift(tmp[x])
      end

      tmp[:query] = query.join('?')

      return super(tmp)
    end

    #
    # == Description
    #
    # Create a new URI::LDAP object from generic URI components as per
    # RFC 2396. No LDAP-specific syntax checking is performed.
    #
    # Arguments are +scheme+, +userinfo+, +host+, +port+, +registry+, +path+,
    # +opaque+, +query+ and +fragment+, in that order.
    #
    # Example:
    #
    #     uri = URI::LDAP.new("ldap", nil, "ldap.example.com", nil,
    #       "/dc=example;dc=com", "query", nil, nil, nil, nil)
    #
    #
    # See also URI::Generic.new
    #
    def initialize(*arg)
      super(*arg)

      if @fragment
        raise InvalidURIError, 'bad LDAP URL'
      end

      parse_dn
      parse_query
    end

    # private method to cleanup +dn+ from using the +path+ component attribute
    def parse_dn
      @dn = @path[1..-1]
    end
    private :parse_dn

    # private method to cleanup +attributes+, +scope+, +filter+ and +extensions+,
    # from using the +query+ component attribute
    def parse_query
      @attributes = nil
      @scope      = nil
      @filter     = nil
      @extensions = nil

      if @query
        attrs, scope, filter, extensions = @query.split('?')

        @attributes = attrs if attrs && attrs.size > 0
        @scope      = scope if scope && scope.size > 0
        @filter     = filter if filter && filter.size > 0
        @extensions = extensions if extensions && extensions.size > 0
      end
    end
    private :parse_query

    # private method to assemble +query+ from +attributes+, +scope+, +filter+ and +extensions+.
    def build_path_query
      @path = '/' + @dn

      query = []
      [@extensions, @filter, @scope, @attributes].each do |x|
        next if !x && query.size == 0
        query.unshift(x)
      end
      @query = query.join('?')
    end
    private :build_path_query

    # returns dn.
    def dn
      @dn
    end

    # private setter for dn +val+
    def set_dn(val)
      @dn = val
      build_path_query
      @dn
    end
    protected :set_dn

    # setter for dn +val+
    def dn=(val)
      set_dn(val)
      val
    end

    # returns attributes.
    def attributes
      @attributes
    end

    # private setter for attributes +val+
    def set_attributes(val)
      @attributes = val
      build_path_query
      @attributes
    end
    protected :set_attributes

    # setter for attributes +val+
    def attributes=(val)
      set_attributes(val)
      val
    end

    # returns scope.
    def scope
      @scope
    end

    # private setter for scope +val+
    def set_scope(val)
      @scope = val
      build_path_query
      @scope
    end
    protected :set_scope

    # setter for scope +val+
    def scope=(val)
      set_scope(val)
      val
    end

    # returns filter.
    def filter
      @filter
    end

    # private setter for filter +val+
    def set_filter(val)
      @filter = val
      build_path_query
      @filter
    end
    protected :set_filter

    # setter for filter +val+
    def filter=(val)
      set_filter(val)
      val
    end

    # returns extensions.
    def extensions
      @extensions
    end

    # private setter for extensions +val+
    def set_extensions(val)
      @extensions = val
      build_path_query
      @extensions
    end
    protected :set_extensions

    # setter for extensions +val+
    def extensions=(val)
      set_extensions(val)
      val
    end

    # Checks if URI has a path
    # For URI::LDAP this will return +false+
    def hierarchical?
      false
    end
  end

  @@schemes['LDAP'] = LDAP
end
