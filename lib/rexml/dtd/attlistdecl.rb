require "rexml/child"
module REXML
  module DTD
    class AttlistDecl < Child
      START = "<!ATTLIST"
      START_RE = /^\s*#{START}/um
      PATTERN_RE = /\s*(#{START}.*?>)/um
    end
  end
end
