# -*- mode: ruby; ruby-indent-level: 4; tab-width: 4 -*- vim: sw=4 ts=4
# $Id$
#
# = yaml.rb: top-level module with methods for loading and parsing YAML documents
#
# Author:: why the lucky stiff
#

require 'yaml/syck'

# == YAML
#
# YAML(tm) (rhymes with 'camel') is a
# straightforward machine parsable data serialization format designed for
# human readability and interaction with scripting languages such as Perl
# and Python. YAML is optimized for data serialization, formatted
# dumping, configuration files, log files, Internet messaging and
# filtering. This specification describes the YAML information model and
# serialization format. Together with the Unicode standard for characters, it
# provides all the information necessary to understand YAML Version 1.0
# and construct computer programs to process it.
#
# See http://yaml.org/ for more information.  For a quick tutorial, please
# visit YAML In Five Minutes (http://yaml.kwiki.org/?YamlInFiveMinutes).
#
# == About This Library
#
# The YAML 1.0 specification outlines four stages of YAML loading and dumping.
# This library honors all four of those stages, although data is really only
# available to you in three stages.
#
# The four stages are: native, representation, serialization, and presentation.
#
# The native stage refers to data which has been loaded completely into Ruby's
# own types. (See +YAML::load+.)
#
# The representation stage means data which has been composed into
# +YAML::BaseNode+ objects.  In this stage, the document is available as a
# tree of node objects.  You can perform YPath queries and transformations
# at this level.  (See +YAML::parse+.)
#
# The serialization stage happens inside the parser.  The YAML parser used in
# Ruby is called Syck.  Serialized nodes are available in the extension as
# SyckNode structs.
#
# The presentation stage is the YAML document itself.  This is accessible
# to you as a string.  (See +YAML::dump+.)
#
# For more information about the various information models, see Chapter
# 3 of the YAML 1.0 Specification (http://yaml.org/spec/#id2491269).
#
# The YAML module provides quick access to the most common loading (YAML::load)
# and dumping (YAML::dump) tasks.  This module also provides an API for registering
# global types (YAML::add_domain_type).
#
# == Example
#
# A simple round-trip (load and dump) of an object.
#
#     require "yaml"
#
#     test_obj = ["dogs", "cats", "badgers"]
#
#     yaml_obj = YAML::dump( test_obj )
#                         # -> ---
#                              - dogs
#                              - cats
#                              - badgers
#     ruby_obj = YAML::load( yaml_obj )
#                         # => ["dogs", "cats", "badgers"]
#     ruby_obj == test_obj
#                         # => true
#
# To register your custom types with the global resolver, use +add_domain_type+.
#
#     YAML::add_domain_type( "your-site.com,2004", "widget" ) do |type, val|
#         Widget.new( val )
#     end
#
module Syck

    DefaultResolver.use_types_at( @@tagged_classes )

    # Returns a new default parser
    def self.parser; Parser.new.set_resolver( self.resolver ); end

    # Returns a new generic parser
    def self.generic_parser
        warn "#{caller[0]}: YAML.generic_parser is deprecated, switch to psych" if $VERBOSE && !caller[0].start_with?(File.dirname(__FILE__))
        Parser.new.set_resolver( GenericResolver )
    end

    # Returns the default resolver
    def self.resolver
        warn "#{caller[0]}: YAML.resolver is deprecated" if $VERBOSE && !caller[0].start_with?(File.dirname(__FILE__))
        DefaultResolver
    end

    # Returns a new default emitter
    def self.emitter
        warn "#{caller[0]}: YAML.emitter is deprecated" if $VERBOSE && !caller[0].start_with?(File.dirname(__FILE__))
        Emitter.new.set_resolver( self.resolver )
    end

    #
    # Converts _obj_ to YAML and writes the YAML result to _io_.
    #
    #   File.open( 'animals.yaml', 'w' ) do |out|
    #     YAML.dump( ['badger', 'elephant', 'tiger'], out )
    #   end
    #
    # If no _io_ is provided, a string containing the dumped YAML
    # is returned.
    #
    #   YAML.dump( :locked )
    #      #=> "--- :locked"
    #
    def self.dump( obj, io = nil )
        obj.to_yaml( io || io2 = StringIO.new )
        io || ( io2.rewind; io2.read )
    end

    #
    # Load a document from the current _io_ stream.
    #
    #   File.open( 'animals.yaml' ) { |yf| YAML::load( yf ) }
    #      #=> ['badger', 'elephant', 'tiger']
    #
    # Can also load from a string.
    #
    #   YAML.load( "--- :locked" )
    #      #=> :locked
    #
    def self.load( io )
        parser.load( io )
    end

    #
    # Load a document from the file located at _filepath_.
    #
    #   YAML.load_file( 'animals.yaml' )
    #      #=> ['badger', 'elephant', 'tiger']
    #
    def self.load_file( filepath )
        File.open( filepath ) do |f|
            load( f )
        end
    end

    #
    # Parse the first document from the current _io_ stream
    #
    #   File.open( 'animals.yaml' ) { |yf| YAML::load( yf ) }
    #      #=> #<YAML::Syck::Node:0x82ccce0
    #           @kind=:seq,
    #           @value=
    #            [#<YAML::Syck::Node:0x82ccd94
    #              @kind=:scalar,
    #              @type_id="str",
    #              @value="badger">,
    #             #<YAML::Syck::Node:0x82ccd58
    #              @kind=:scalar,
    #              @type_id="str",
    #              @value="elephant">,
    #             #<YAML::Syck::Node:0x82ccd1c
    #              @kind=:scalar,
    #              @type_id="str",
    #              @value="tiger">]>
    #
    # Can also load from a string.
    #
    #   YAML.parse( "--- :locked" )
    #      #=> #<YAML::Syck::Node:0x82edddc
    #            @type_id="tag:ruby.yaml.org,2002:sym",
    #            @value=":locked", @kind=:scalar>
    #
    def self.parse( io )
        generic_parser.load( io )
    end

    #
    # Parse a document from the file located at _filepath_.
    #
    #   YAML.parse_file( 'animals.yaml' )
    #      #=> #<YAML::Syck::Node:0x82ccce0
    #           @kind=:seq,
    #           @value=
    #            [#<YAML::Syck::Node:0x82ccd94
    #              @kind=:scalar,
    #              @type_id="str",
    #              @value="badger">,
    #             #<YAML::Syck::Node:0x82ccd58
    #              @kind=:scalar,
    #              @type_id="str",
    #              @value="elephant">,
    #             #<YAML::Syck::Node:0x82ccd1c
    #              @kind=:scalar,
    #              @type_id="str",
    #              @value="tiger">]>
    #
    def self.parse_file( filepath )
        File.open( filepath ) do |f|
            parse( f )
        end
    end

    #
    # Calls _block_ with each consecutive document in the YAML
    # stream contained in _io_.
    #
    #   File.open( 'many-docs.yaml' ) do |yf|
    #     YAML.each_document( yf ) do |ydoc|
    #       ## ydoc contains the single object
    #       ## from the YAML document
    #     end
    #   end
    #
    def self.each_document( io, &block )
        warn "#{caller[0]}: YAML.each_document is deprecated" if $VERBOSE && !caller[0].start_with?(File.dirname(__FILE__))
        parser.load_documents( io, &block )
    end

    #
    # Calls _block_ with each consecutive document in the YAML
    # stream contained in _io_.
    #
    #   File.open( 'many-docs.yaml' ) do |yf|
    #     YAML.load_documents( yf ) do |ydoc|
    #       ## ydoc contains the single object
    #       ## from the YAML document
    #     end
    #   end
    #
    def self.load_documents( io, &doc_proc )
        parser.load_documents( io, &doc_proc )
    end

    #
    # Calls _block_ with a tree of +YAML::BaseNodes+, one tree for
    # each consecutive document in the YAML stream contained in _io_.
    #
    #   File.open( 'many-docs.yaml' ) do |yf|
    #     YAML.each_node( yf ) do |ydoc|
    #       ## ydoc contains a tree of nodes
    #       ## from the YAML document
    #     end
    #   end
    #
    def self.each_node( io, &doc_proc )
        warn "#{caller[0]}: YAML.each_node is deprecated" if $VERBOSE && !caller[0].start_with?(File.dirname(__FILE__))
        generic_parser.load_documents( io, &doc_proc )
    end

    #
    # Calls _block_ with a tree of +YAML::BaseNodes+, one tree for
    # each consecutive document in the YAML stream contained in _io_.
    #
    #   File.open( 'many-docs.yaml' ) do |yf|
    #     YAML.parse_documents( yf ) do |ydoc|
    #       ## ydoc contains a tree of nodes
    #       ## from the YAML document
    #     end
    #   end
    #
    def self.parse_documents( io, &doc_proc )
        warn "#{caller[0]}: YAML.parse_documents is deprecated, use load_stream" if $VERBOSE && !caller[0].start_with?(File.dirname(__FILE__))
        self.each_node( io, &doc_proc )
    end

    #
    # Loads all documents from the current _io_ stream,
    # returning a +YAML::Stream+ object containing all
    # loaded documents.
    #
    def self.load_stream( io )
        d = nil
        parser.load_documents( io ) do |doc|
            d = Stream.new if not d
            d.add( doc )
        end
        return d
    end

    #
    # Returns a YAML stream containing each of the items in +objs+,
    # each having their own document.
    #
    #   YAML.dump_stream( 0, [], {} )
    #     #=> --- 0
    #         --- []
    #         --- {}
    #
    def self.dump_stream( *objs )
        d = Stream.new
        objs.each do |doc|
            d.add( doc )
        end
        d.emit
    end

    #
    # Add a global handler for a YAML domain type.
    #
    def self.add_domain_type( domain, type_tag, &transfer_proc )
        resolver.add_type( "tag:#{ domain }:#{ type_tag }", transfer_proc )
    end

    #
    # Add a transfer method for a builtin type
    #
    def self.add_builtin_type( type_tag, &transfer_proc )
        resolver.add_type( "tag:yaml.org,2002:#{ type_tag }", transfer_proc )
    end

    #
    # Add a transfer method for a builtin type
    #
    def self.add_ruby_type( type_tag, &transfer_proc )
        warn "#{caller[0]}: YAML.add_ruby_type is deprecated, use add_domain_type" if $VERBOSE && !caller[0].start_with?(File.dirname(__FILE__))
        resolver.add_type( "tag:ruby.yaml.org,2002:#{ type_tag }", transfer_proc )
    end

    #
    # Add a private document type
    #
    def self.add_private_type( type_re, &transfer_proc )
        warn "#{caller[0]}: YAML.add_private_type is deprecated, use add_domain_type" if $VERBOSE && !caller[0].start_with?(File.dirname(__FILE__))
        resolver.add_type( "x-private:" + type_re, transfer_proc )
    end

    #
    # Detect typing of a string
    #
    def self.detect_implicit( val )
        warn "#{caller[0]}: YAML.detect_implicit is deprecated" if $VERBOSE && !caller[0].start_with?(File.dirname(__FILE__))
        resolver.detect_implicit( val )
    end

    #
    # Convert a type_id to a taguri
    #
    def self.tagurize( val )
        warn "#{caller[0]}: YAML.tagurize is deprecated" if $VERBOSE && !caller[0].start_with?(File.dirname(__FILE__))
        resolver.tagurize( val )
    end

    #
    # Apply a transfer method to a Ruby object
    #
    def self.transfer( type_id, obj )
        warn "#{caller[0]}: YAML.transfer is deprecated" if $VERBOSE && !caller[0].start_with?(File.dirname(__FILE__))
        resolver.transfer( tagurize( type_id ), obj )
    end

    #
    # Apply any implicit a node may qualify for
    #
    def self.try_implicit( obj )
        warn "#{caller[0]}: YAML.try_implicit is deprecated" if $VERBOSE && !caller[0].start_with?(File.dirname(__FILE__))
        transfer( detect_implicit( obj ), obj )
    end

    #
    # Method to extract colon-seperated type and class, returning
    # the type and the constant of the class
    #
    def self.read_type_class( type, obj_class )
        warn "#{caller[0]}: YAML.read_type_class is deprecated" if $VERBOSE && !caller[0].start_with?(File.dirname(__FILE__))
        type, tclass = type.split( ':', 4 ).last(2)
        tclass.split( "::" ).each { |c| obj_class = obj_class.const_get( c ) } if tclass
        return [ type, obj_class ]
    end

    #
    # Allocate blank object
    #
    def self.object_maker( obj_class, val )
        warn "#{caller[0]}: YAML.object_maker is deprecated" if $VERBOSE && !caller[0].start_with?(File.dirname(__FILE__))
        if Hash === val
            o = obj_class.allocate
            val.each_pair { |k,v|
                o.instance_variable_set("@#{k}", v)
            }
            o
        else
            raise Error, "Invalid object explicitly tagged !ruby/Object: " + val.inspect
        end
    end

    #
    # Allocate an Emitter if needed
    #
    def self.quick_emit( oid, opts = {}, &e )
        warn "#{caller[0]}: YAML.quick_emit is deprecated" if $VERBOSE && !caller[0].start_with?(File.dirname(__FILE__))
        out =
            if opts.is_a? Emitter
                opts
            else
                emitter.reset( opts )
            end
        out.emit( oid, &e )
    end

end

module Kernel
    #
    # ryan:: You know how Kernel.p is a really convenient way to dump ruby
    #        structures?  The only downside is that it's not as legible as
    #        YAML.
    #
    # _why:: (listening)
    #
    # ryan:: I know you don't want to urinate all over your users' namespaces.
    #        But, on the other hand, convenience of dumping for debugging is,
    #        IMO, a big YAML use case.
    #
    # _why:: Go nuts!  Have a pony parade!
    #
    # ryan:: Either way, I certainly will have a pony parade.
    #

    # Prints any supplied _objects_ out in YAML.  Intended as
    # a variation on +Kernel::p+.
    #
    #   S = Struct.new(:name, :state)
    #   s = S['dave', 'TX']
    #   y s
    #
    # _produces:_
    #
    #   --- !ruby/struct:S
    #   name: dave
    #   state: TX
    #
    def y( object, *objects )
        objects.unshift object
        puts( if objects.length == 1
                  YAML.dump( *objects )
              else
                  YAML.dump_stream( *objects )
              end )
    end
    private :y
end


