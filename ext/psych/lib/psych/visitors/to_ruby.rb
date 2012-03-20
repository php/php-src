require 'psych/scalar_scanner'

unless defined?(Regexp::NOENCODING)
  Regexp::NOENCODING = 32
end

module Psych
  module Visitors
    ###
    # This class walks a YAML AST, converting each node to ruby
    class ToRuby < Psych::Visitors::Visitor
      def initialize ss = ScalarScanner.new
        super()
        @st = {}
        @ss = ss
        @domain_types = Psych.domain_types
      end

      def accept target
        result = super
        return result if @domain_types.empty? || !target.tag

        key = target.tag.sub(/^[!\/]*/, '').sub(/(,\d+)\//, '\1:')
        key = "tag:#{key}" unless key =~ /^(tag:|x-private)/

        if @domain_types.key? key
          value, block = @domain_types[key]
          return block.call value, result
        end

        result
      end

      def deserialize o
        if klass = Psych.load_tags[o.tag]
          instance = klass.allocate

          if instance.respond_to?(:init_with)
            coder = Psych::Coder.new(o.tag)
            coder.scalar = o.value
            instance.init_with coder
          end

          return instance
        end

        return o.value if o.quoted
        return @ss.tokenize(o.value) unless o.tag

        case o.tag
        when '!binary', 'tag:yaml.org,2002:binary'
          o.value.unpack('m').first
        when /^!(?:str|ruby\/string)(?::(.*))?/, 'tag:yaml.org,2002:str'
          klass = resolve_class($1)
          if klass
            klass.allocate.replace o.value
          else
            o.value
          end
        when '!ruby/object:BigDecimal'
          require 'bigdecimal'
          BigDecimal._load o.value
        when "!ruby/object:DateTime"
          require 'date'
          @ss.parse_time(o.value).to_datetime
        when "!ruby/object:Complex"
          Complex(o.value)
        when "!ruby/object:Rational"
          Rational(o.value)
        when "!ruby/class", "!ruby/module"
          resolve_class o.value
        when "tag:yaml.org,2002:float", "!float"
          Float(@ss.tokenize(o.value))
        when "!ruby/regexp"
          o.value =~ /^\/(.*)\/([mixn]*)$/
          source  = $1
          options = 0
          lang    = nil
          ($2 || '').split('').each do |option|
            case option
            when 'x' then options |= Regexp::EXTENDED
            when 'i' then options |= Regexp::IGNORECASE
            when 'm' then options |= Regexp::MULTILINE
            when 'n' then options |= Regexp::NOENCODING
            else lang = option
            end
          end
          Regexp.new(*[source, options, lang].compact)
        when "!ruby/range"
          args = o.value.split(/([.]{2,3})/, 2).map { |s|
            accept Nodes::Scalar.new(s)
          }
          args.push(args.delete_at(1) == '...')
          Range.new(*args)
        when /^!ruby\/sym(bol)?:?(.*)?$/
          o.value.to_sym
        else
          @ss.tokenize o.value
        end
      end
      private :deserialize

      def visit_Psych_Nodes_Scalar o
        register o, deserialize(o)
      end

      def visit_Psych_Nodes_Sequence o
        if klass = Psych.load_tags[o.tag]
          instance = klass.allocate

          if instance.respond_to?(:init_with)
            coder = Psych::Coder.new(o.tag)
            coder.seq = o.children.map { |c| accept c }
            instance.init_with coder
          end

          return instance
        end

        case o.tag
        when '!omap', 'tag:yaml.org,2002:omap'
          map = register(o, Psych::Omap.new)
          o.children.each { |a|
            map[accept(a.children.first)] = accept a.children.last
          }
          map
        when /^!(?:seq|ruby\/array):(.*)$/
          klass = resolve_class($1)
          list  = register(o, klass.allocate)
          o.children.each { |c| list.push accept c }
          list
        else
          list = register(o, [])
          o.children.each { |c| list.push accept c }
          list
        end
      end

      def visit_Psych_Nodes_Mapping o
        return revive(Psych.load_tags[o.tag], o) if Psych.load_tags[o.tag]
        return revive_hash({}, o) unless o.tag

        case o.tag
        when /^!(?:str|ruby\/string)(?::(.*))?/, 'tag:yaml.org,2002:str'
          klass = resolve_class($1)
          members = Hash[*o.children.map { |c| accept c }]
          string = members.delete 'str'

          if klass
            string = klass.allocate
            string.replace string
          end

          init_with(string, members.map { |k,v| [k.to_s.sub(/^@/, ''),v] }, o)
        when /^!ruby\/array:(.*)$/
          klass = resolve_class($1)
          list  = register(o, klass.allocate)

          members = Hash[o.children.map { |c| accept c }.each_slice(2).to_a]
          list.replace members['internal']

          members['ivars'].each do |ivar, v|
            list.instance_variable_set ivar, v
          end
          list
        when /^!ruby\/struct:?(.*)?$/
          klass = resolve_class($1)

          if klass
            s = register(o, klass.allocate)

            members = {}
            struct_members = s.members.map { |x| x.to_sym }
            o.children.each_slice(2) do |k,v|
              member = accept(k)
              value  = accept(v)
              if struct_members.include?(member.to_sym)
                s.send("#{member}=", value)
              else
                members[member.to_s.sub(/^@/, '')] = value
              end
            end
            init_with(s, members, o)
          else
            members = o.children.map { |c| accept c }
            h = Hash[*members]
            Struct.new(*h.map { |k,v| k.to_sym }).new(*h.map { |k,v| v })
          end

        when '!ruby/range'
          h = Hash[*o.children.map { |c| accept c }]
          register o, Range.new(h['begin'], h['end'], h['excl'])

        when /^!ruby\/exception:?(.*)?$/
          h = Hash[*o.children.map { |c| accept c }]

          e = build_exception((resolve_class($1) || Exception),
                              h.delete('message'))
          init_with(e, h, o)

        when '!set', 'tag:yaml.org,2002:set'
          set = Psych::Set.new
          @st[o.anchor] = set if o.anchor
          o.children.each_slice(2) do |k,v|
            set[accept(k)] = accept(v)
          end
          set

        when '!ruby/object:Complex'
          h = Hash[*o.children.map { |c| accept c }]
          register o, Complex(h['real'], h['image'])

        when '!ruby/object:Rational'
          h = Hash[*o.children.map { |c| accept c }]
          register o, Rational(h['numerator'], h['denominator'])

        when /^!ruby\/object:?(.*)?$/
          name = $1 || 'Object'
          obj = revive((resolve_class(name) || Object), o)
          obj

        when /^!map:(.*)$/, /^!ruby\/hash:(.*)$/
          revive_hash resolve_class($1).new, o

        else
          revive_hash({}, o)
        end
      end

      def visit_Psych_Nodes_Document o
        accept o.root
      end

      def visit_Psych_Nodes_Stream o
        o.children.map { |c| accept c }
      end

      def visit_Psych_Nodes_Alias o
        @st.fetch(o.anchor) { raise BadAlias, "Unknown alias: #{o.anchor}" }
      end

      private
      def register node, object
        @st[node.anchor] = object if node.anchor
        object
      end

      def revive_hash hash, o
        @st[o.anchor] = hash if o.anchor

          o.children.each_slice(2) { |k,v|
          key = accept(k)

          if key == '<<'
            case v
            when Nodes::Alias
              hash.merge! accept(v)
            when Nodes::Sequence
              accept(v).reverse_each do |value|
                hash.merge! value
              end
            else
              hash[key] = accept(v)
            end
          else
            hash[key] = accept(v)
          end

        }
        hash
      end

      def revive klass, node
        s = klass.allocate
        @st[node.anchor] = s if node.anchor
        h = Hash[*node.children.map { |c| accept c }]
        init_with(s, h, node)
      end

      def init_with o, h, node
        c = Psych::Coder.new(node.tag)
        c.map = h

        if o.respond_to?(:init_with)
          o.init_with c
        elsif o.respond_to?(:yaml_initialize)
          if $VERBOSE
            "Implementing #{o.class}#yaml_initialize is deprecated, please implement \"init_with(coder)\""
          end
          o.yaml_initialize c.tag, c.map
        else
          h.each { |k,v| o.instance_variable_set(:"@#{k}", v) }
        end
        o
      end

      # Convert +klassname+ to a Class
      def resolve_class klassname
        return nil unless klassname and not klassname.empty?

        name    = klassname
        retried = false

        begin
          path2class(name)
        rescue ArgumentError, NameError => ex
          unless retried
            name    = "Struct::#{name}"
            retried = ex
            retry
          end
          raise retried
        end
      end
    end
  end
end
