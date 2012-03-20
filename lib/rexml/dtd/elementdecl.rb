require "rexml/child"
module REXML
  module DTD
    class ElementDecl < Child
      START = "<!ELEMENT"
      START_RE = /^\s*#{START}/um
      PATTERN_RE = /^\s*(#{START}.*?)>/um
      PATTERN_RE = /^\s*#{START}\s+((?:[:\w_][-\.\w_]*:)?[-!\*\.\w_]*)(.*?)>/
      #\s*((((["']).*?\5)|[^\/'">]*)*?)(\/)?>/um, true)

      def initialize match
        @name = match[1]
        @rest = match[2]
      end
    end
  end
end
