#--
#
# $RCSfile$
#
# = Ruby-space definitions that completes C-space funcs for X509 and subclasses
# 
# = Info
# 'OpenSSL for Ruby 2' project
# Copyright (C) 2002  Michal Rokos <m.rokos@sh.cvut.cz>
# All rights reserved.
# 
# = Licence
# This program is licenced under the same licence as Ruby.
# (See the file 'LICENCE'.)
# 
# = Version
# $Id$
#
#++

module OpenSSL
  module X509
    class ExtensionFactory
      def create_extension(*arg)
        if arg.size > 1
          create_ext(*arg)
        else
          send("create_ext_from_"+arg[0].class.name.downcase, arg[0])
        end
      end

      def create_ext_from_array(ary)
        raise ExtensionError, "unexpected array form" if ary.size > 3
        create_ext(ary[0], ary[1], ary[2])
      end

      def create_ext_from_string(str) # "oid = critical, value"
        oid, value = str.split(/=/, 2)
        oid.strip!
        value.strip!
        create_ext(oid, value)
      end

      def create_ext_from_hash(hash)
        create_ext(hash["oid"], hash["value"], hash["critical"])
      end
    end

    class Extension
      def to_s # "oid = critical, value"
        str = self.oid
        str << " = "
        str << "critical, " if self.critical?
        str << self.value.gsub(/\n/, ", ")
      end

      def to_h # {"oid"=>sn|ln, "value"=>value, "critical"=>true|false}
        {"oid"=>self.oid,"value"=>self.value,"critical"=>self.critical?}
      end

      def to_a
        [ self.oid, self.value, self.critical? ]
      end
    end

    class Name
      module RFC2253DN
        Special = ',=+<>#;'
        HexChar = /[0-9a-fA-F]/
        HexPair = /#{HexChar}#{HexChar}/
        HexString = /#{HexPair}+/
        Pair = /\\(?:[#{Special}]|\\|"|#{HexPair})/
        StringChar = /[^#{Special}\\"]/
        QuoteChar = /[^\\"]/
        AttributeType = /[a-zA-Z][0-9a-zA-Z]*|[0-9]+(?:\.[0-9]+)*/
        AttributeValue = /
          (?!["#])((?:#{StringChar}|#{Pair})*)|
          \#(#{HexString})|
          "((?:#{QuoteChar}|#{Pair})*)"
        /x
        TypeAndValue = /\A(#{AttributeType})=#{AttributeValue}/

        module_function

        def expand_pair(str)
          return nil unless str
          return str.gsub(Pair){
            pair = $&
            case pair.size
            when 2 then pair[1,1]
            when 3 then Integer("0x#{pair[1,2]}").chr
            else raise OpenSSL::X509::NameError, "invalid pair: #{str}"
            end
          }
        end

        def expand_hexstring(str)
          return nil unless str
          der = str.gsub(HexPair){$&.to_i(16).chr }
          a1 = OpenSSL::ASN1.decode(der)
          return a1.value, a1.tag
        end

        def expand_value(str1, str2, str3)
          value = expand_pair(str1)
          value, tag = expand_hexstring(str2) unless value
          value = expand_pair(str3) unless value
          return value, tag
        end

        def scan(dn)
          str = dn
          ary = []
          while true
            if md = TypeAndValue.match(str)
              remain = md.post_match
              type = md[1]
              value, tag = expand_value(md[2], md[3], md[4]) rescue nil
              if value
                type_and_value = [type, value]
                type_and_value.push(tag) if tag
                ary.unshift(type_and_value)
                if remain.length > 2 && remain[0] == ?,
                  str = remain[1..-1]
                  next
                elsif remain.length > 2 && remain[0] == ?+
                  raise OpenSSL::X509::NameError,
                    "multi-valued RDN is not supported: #{dn}"
                elsif remain.empty?
                  break
                end
              end
            end
            msg_dn = dn[0, dn.length - str.length] + " =>" + str
            raise OpenSSL::X509::NameError, "malformed RDN: #{msg_dn}"
          end
          return ary
        end
      end

      class << self
        def parse_rfc2253(str, template=OBJECT_TYPE_TEMPLATE)
          ary = OpenSSL::X509::Name::RFC2253DN.scan(str)
          self.new(ary, template)
        end

        def parse_openssl(str, template=OBJECT_TYPE_TEMPLATE)
          ary = str.scan(/\s*([^\/,]+)\s*/).collect{|i| i[0].split("=", 2) }
          self.new(ary, template)
        end

        alias parse parse_openssl
      end
    end

    class StoreContext
      def cleanup
        warn "(#{caller.first}) OpenSSL::X509::StoreContext#cleanup is deprecated with no replacement" if $VERBOSE
      end
    end
  end
end
