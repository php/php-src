# -*- mode: ruby; ruby-indent-level: 4 -*- vim: sw=4
#
# Classes required by the full core typeset
#

module Syck

    #
    # Default private type
    #
    class PrivateType
        def self.tag_subclasses?; false; end
        verbose, $VERBOSE = $VERBOSE, nil
        def initialize( type, val )
            @type_id = type; @value = val
            @value.taguri = "x-private:#{ @type_id }"
        end
        def to_yaml( opts = {} )
            @value.to_yaml( opts )
        end
    ensure
        $VERBOSE = verbose
    end

    #
    # Default domain type
    #
    class DomainType
        def self.tag_subclasses?; false; end
        verbose, $VERBOSE = $VERBOSE, nil
        def initialize( domain, type, val )
            @domain = domain; @type_id = type; @value = val
            @value.taguri = "tag:#{ @domain }:#{ @type_id }"
        end
        def to_yaml( opts = {} )
            @value.to_yaml( opts )
        end
    ensure
        $VERBOSE = verbose
    end

    #
    # Unresolved objects
    #
    class Object
        def self.tag_subclasses?; false; end
        def to_yaml( opts = {} )
            ::Syck.quick_emit( self, opts ) do |out|
                out.map( "tag:ruby.yaml.org,2002:object:#{ @class }", to_yaml_style ) do |map|
                    @ivars.each do |k,v|
                        map.add( k, v )
                    end
                end
            end
        end
    end

    #
    # YAML Hash class to support comments and defaults
    #
    class SpecialHash < ::Hash
        attr_accessor :default
        def inspect
            self.default.to_s
        end
        def to_s
            self.default.to_s
        end
        def update( h )
            if ::Syck::SpecialHash === h
                @default = h.default if h.default
            end
            super( h )
        end
        def to_yaml( opts = {} )
            opts[:DefaultKey] = self.default
            super( opts )
        end
    end

    #
    # Builtin collection: !omap
    #
    class Omap < ::Array
        yaml_as "tag:yaml.org,2002:omap"
        def yaml_initialize( tag, val )
            if Array === val
                val.each do |v|
                    if Hash === v
                        concat( v.to_a )		# Convert the map to a sequence
                    else
                        raise ::Syck::Error, "Invalid !omap entry: " + val.inspect
                    end
                end
            else
                raise ::Syck::Error, "Invalid !omap: " + val.inspect
            end
            self
        end
        def self.[]( *vals )
            o = Omap.new
            0.step( vals.length - 1, 2 ) do |i|
                o[vals[i]] = vals[i+1]
            end
            o
        end
        def []( k )
            self.assoc( k ).to_a[1]
        end
        def []=( k, *rest )
            val, set = rest.reverse
            if ( tmp = self.assoc( k ) ) and not set
                tmp[1] = val
            else
                self << [ k, val ]
            end
            val
        end
        def has_key?( k )
            self.assoc( k ) ? true : false
        end
        def is_complex_yaml?
            true
        end
        def to_yaml( opts = {} )
            ::Syck.quick_emit( self, opts ) do |out|
                out.seq( taguri, to_yaml_style ) do |seq|
                    self.each do |v|
                        seq.add( Hash[ *v ] )
                    end
                end
            end
        end
    end

    #
    # Builtin collection: !pairs
    #
    class Pairs < ::Array
        yaml_as "tag:yaml.org,2002:pairs"
        def yaml_initialize( tag, val )
            if Array === val
                val.each do |v|
                    if Hash === v
                        concat( v.to_a )		# Convert the map to a sequence
                    else
                        raise ::Syck::Error, "Invalid !pairs entry: " + val.inspect
                    end
                end
            else
                raise ::Syck::Error, "Invalid !pairs: " + val.inspect
            end
            self
        end
        def self.[]( *vals )
            p = Pairs.new
            0.step( vals.length - 1, 2 ) { |i|
                p[vals[i]] = vals[i+1]
            }
            p
        end
        def []( k )
            self.assoc( k ).to_a
        end
        def []=( k, val )
            self << [ k, val ]
            val
        end
        def has_key?( k )
            self.assoc( k ) ? true : false
        end
        def is_complex_yaml?
            true
        end
        def to_yaml( opts = {} )
            ::Syck.quick_emit( self, opts ) do |out|
                out.seq( taguri, to_yaml_style ) do |seq|
                    self.each do |v|
                        seq.add( Hash[ *v ] )
                    end
                end
            end
        end
    end

    #
    # Builtin collection: !set
    #
    class Set < ::Hash
        yaml_as "tag:yaml.org,2002:set"
    end
end
