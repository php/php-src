#
# YAML::Loader class
# .. type handling ..
#
module Syck
    class Loader
        TRANSFER_DOMAINS = {
            'yaml.org,2002' => {},
            'ruby.yaml.org,2002' => {}
        }
        PRIVATE_TYPES = {}
        IMPLICIT_TYPES = [ 'null', 'bool', 'time', 'int', 'float' ]
    end
end
