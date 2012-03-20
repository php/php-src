#
# $Id$
# Configuration file for XML-RPC for Ruby
#

module XMLRPC

  module Config

    DEFAULT_WRITER = XMLWriter::Simple            # or XMLWriter::XMLParser

    # available parser:
    #   * XMLParser::NQXMLTreeParser
    #   * XMLParser::NQXMLStreamParser
    #   * XMLParser::XMLTreeParser
    #   * XMLParser::XMLStreamParser (fastest)
    #   * XMLParser::REXMLStreamParser
    #   * XMLParser::XMLScanStreamParser
    DEFAULT_PARSER = XMLParser::REXMLStreamParser

    # enable <nil/> tag
    ENABLE_NIL_CREATE    = false
    ENABLE_NIL_PARSER    = false

    # allows integers greater than 32-bit if true
    ENABLE_BIGINT        = false

    # enable marshalling ruby objects which include XMLRPC::Marshallable
    ENABLE_MARSHALLING   = true

    # enable multiCall extension by default
    ENABLE_MULTICALL     = false

    # enable Introspection extension by default
    ENABLE_INTROSPECTION = false

  end

end

