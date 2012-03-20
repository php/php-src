module REXML
  # Defines a number of tokens used for parsing XML.  Not for general
  # consumption.
  module XMLTokens
    NCNAME_STR= '[\w:][\-\w.]*'
    NAME_STR= "(?:#{NCNAME_STR}:)?#{NCNAME_STR}"

    NAMECHAR = '[\-\w\.:]'
    NAME = "([\\w:]#{NAMECHAR}*)"
    NMTOKEN = "(?:#{NAMECHAR})+"
    NMTOKENS = "#{NMTOKEN}(\\s+#{NMTOKEN})*"
    REFERENCE = "(?:&#{NAME};|&#\\d+;|&#x[0-9a-fA-F]+;)"

    #REFERENCE = "(?:#{ENTITYREF}|#{CHARREF})"
    #ENTITYREF = "&#{NAME};"
    #CHARREF = "&#\\d+;|&#x[0-9a-fA-F]+;"
  end
end
