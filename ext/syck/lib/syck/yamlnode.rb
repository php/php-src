#
# YAML::YamlNode class
#
require 'syck/basenode'

module Syck

    #
    # YAML Generic Model container
    #
    class YamlNode
        include BaseNode
        attr_accessor :kind, :type_id, :value, :anchor
        def initialize(t, v)
            @type_id = t
            if Hash === v
                @kind = 'map'
                @value = {}
                v.each {|key,val|
                    @value[key.transform] = [key, val]
                }
            elsif Array === v
                @kind = 'seq'
                @value = v
            elsif String === v
                @kind = 'scalar'
                @value = v
            end
        end

        #
        # Transform this node fully into a native type
        #
        def transform
            t = nil
            if @value.is_a? Hash
                t = {}
                @value.each { |k,v|
                    t[ k ] = v[1].transform
                }
            elsif @value.is_a? Array
                t = []
                @value.each { |v|
                    t.push v.transform
                }
            else
                t = @value
            end
            Syck.transfer_method( @type_id, t )
        end

    end

end
