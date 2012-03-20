module Psych
  module Visitors
    ###
    # YAMLTree builds a YAML ast given a ruby object.  For example:
    #
    #   builder = Psych::Visitors::YAMLTree.new
    #   builder << { :foo => 'bar' }
    #   builder.tree # => #<Psych::Nodes::Stream .. }
    #
    class YAMLTree < Psych::Visitors::Visitor
      attr_reader :started, :finished
      alias :finished? :finished
      alias :started? :started

      def initialize options = {}, emitter = TreeBuilder.new, ss = ScalarScanner.new
        super()
        @started  = false
        @finished = false
        @emitter  = emitter
        @st       = {}
        @ss       = ss
        @options  = options

        @dispatch_cache = Hash.new do |h,klass|
          method = "visit_#{(klass.name || '').split('::').join('_')}"

          method = respond_to?(method) ? method : h[klass.superclass]

          raise(TypeError, "Can't dump #{target.class}") unless method

          h[klass] = method
        end
      end

      def start encoding = Nodes::Stream::UTF8
        @emitter.start_stream(encoding).tap do
          @started = true
        end
      end

      def finish
        @emitter.end_stream.tap do
          @finished = true
        end
      end

      def tree
        finish unless finished?
      end

      def push object
        start unless started?
        version = []
        version = [1,1] if @options[:header]

        case @options[:version]
        when Array
          version = @options[:version]
        when String
          version = @options[:version].split('.').map { |x| x.to_i }
        else
          version = [1,1]
        end if @options.key? :version

        @emitter.start_document version, [], false
        accept object
        @emitter.end_document
      end
      alias :<< :push

      def accept target
        # return any aliases we find
        if @st.key? target.object_id
          oid         = target.object_id
          node        = @st[oid]
          anchor      = oid.to_s
          node.anchor = anchor
          return @emitter.alias anchor
        end

        if target.respond_to?(:to_yaml)
          begin
            loc = target.method(:to_yaml).source_location.first
            if loc !~ /(syck\/rubytypes.rb|psych\/core_ext.rb)/
              unless target.respond_to?(:encode_with)
                if $VERBOSE
                  warn "implementing to_yaml is deprecated, please implement \"encode_with\""
                end

                target.to_yaml(:nodump => true)
              end
            end
          rescue
            # public_method or source_location might be overridden,
            # and it's OK to skip it since it's only to emit a warning
          end
        end

        if target.respond_to?(:encode_with)
          dump_coder target
        else
          send(@dispatch_cache[target.class], target)
        end
      end

      def visit_Psych_Omap o
        seq = @emitter.start_sequence(nil, '!omap', false, Nodes::Sequence::BLOCK)
        register(o, seq)

        o.each { |k,v| visit_Hash k => v }
        @emitter.end_sequence
      end

      def visit_Object o
        tag = Psych.dump_tags[o.class]
        unless tag
          klass = o.class == Object ? nil : o.class.name
          tag   = ['!ruby/object', klass].compact.join(':')
        end

        map = @emitter.start_mapping(nil, tag, false, Nodes::Mapping::BLOCK)
        register(o, map)

        dump_ivars o
        @emitter.end_mapping
      end

      def visit_Struct o
        tag = ['!ruby/struct', o.class.name].compact.join(':')

        register o, @emitter.start_mapping(nil, tag, false, Nodes::Mapping::BLOCK)
        o.members.each do |member|
          @emitter.scalar member.to_s, nil, nil, true, false, Nodes::Scalar::ANY
          accept o[member]
        end

        dump_ivars o

        @emitter.end_mapping
      end

      def visit_Exception o
        tag = ['!ruby/exception', o.class.name].join ':'

        @emitter.start_mapping nil, tag, false, Nodes::Mapping::BLOCK

        {
          'message'   => private_iv_get(o, 'mesg'),
          'backtrace' => private_iv_get(o, 'backtrace'),
        }.each do |k,v|
          next unless v
          @emitter.scalar k, nil, nil, true, false, Nodes::Scalar::ANY
          accept v
        end

        dump_ivars o

        @emitter.end_mapping
      end

      def visit_Regexp o
        register o, @emitter.scalar(o.inspect, nil, '!ruby/regexp', false, false, Nodes::Scalar::ANY)
      end

      def visit_DateTime o
        formatted = format_time o.to_time
        tag = '!ruby/object:DateTime'
        register o, @emitter.scalar(formatted, nil, tag, false, false, Nodes::Scalar::ANY)
      end

      def visit_Time o
        formatted = format_time o
        @emitter.scalar formatted, nil, nil, true, false, Nodes::Scalar::ANY
      end

      def visit_Rational o
        register o, @emitter.start_mapping(nil, '!ruby/object:Rational', false, Nodes::Mapping::BLOCK)

        [
          'denominator', o.denominator.to_s,
          'numerator', o.numerator.to_s
        ].each do |m|
          @emitter.scalar m, nil, nil, true, false, Nodes::Scalar::ANY
        end

        @emitter.end_mapping
      end

      def visit_Complex o
        register o, @emitter.start_mapping(nil, '!ruby/object:Complex', false, Nodes::Mapping::BLOCK)

        ['real', o.real.to_s, 'image', o.imag.to_s].each do |m|
          @emitter.scalar m, nil, nil, true, false, Nodes::Scalar::ANY
        end

        @emitter.end_mapping
      end

      def visit_Integer o
        @emitter.scalar o.to_s, nil, nil, true, false, Nodes::Scalar::ANY
      end
      alias :visit_TrueClass :visit_Integer
      alias :visit_FalseClass :visit_Integer
      alias :visit_Date :visit_Integer

      def visit_Float o
        if o.nan?
          @emitter.scalar '.nan', nil, nil, true, false, Nodes::Scalar::ANY
        elsif o.infinite?
          @emitter.scalar((o.infinite? > 0 ? '.inf' : '-.inf'),
            nil, nil, true, false, Nodes::Scalar::ANY)
        else
          @emitter.scalar o.to_s, nil, nil, true, false, Nodes::Scalar::ANY
        end
      end

      def visit_BigDecimal o
        @emitter.scalar o._dump, nil, '!ruby/object:BigDecimal', false, false, Nodes::Scalar::ANY
      end

      def binary? string
        string.encoding == Encoding::ASCII_8BIT ||
          string.index("\x00") ||
          string.count("\x00-\x7F", "^ -~\t\r\n").fdiv(string.length) > 0.3
      end
      private :binary?

      def visit_String o
        plain = false
        quote = false
        style = Nodes::Scalar::ANY

        if binary?(o)
          str   = [o].pack('m').chomp
          tag   = '!binary' # FIXME: change to below when syck is removed
          #tag   = 'tag:yaml.org,2002:binary'
          style = Nodes::Scalar::LITERAL
        else
          str   = o
          tag   = nil
          quote = !(String === @ss.tokenize(o))
          plain = !quote
        end

        ivars = find_ivars o

        if ivars.empty?
          unless o.class == ::String
            tag = "!ruby/string:#{o.class}"
          end
          @emitter.scalar str, nil, tag, plain, quote, style
        else
          maptag = '!ruby/string'
          maptag << ":#{o.class}" unless o.class == ::String

          @emitter.start_mapping nil, maptag, false, Nodes::Mapping::BLOCK
          @emitter.scalar 'str', nil, nil, true, false, Nodes::Scalar::ANY
          @emitter.scalar str, nil, tag, plain, quote, style

          dump_ivars o

          @emitter.end_mapping
        end
      end

      def visit_Module o
        raise TypeError, "can't dump anonymous module: #{o}" unless o.name
        register o, @emitter.scalar(o.name, nil, '!ruby/module', false, false, Nodes::Scalar::SINGLE_QUOTED)
      end

      def visit_Class o
        raise TypeError, "can't dump anonymous class: #{o}" unless o.name
        register o, @emitter.scalar(o.name, nil, '!ruby/class', false, false, Nodes::Scalar::SINGLE_QUOTED)
      end

      def visit_Range o
        register o, @emitter.start_mapping(nil, '!ruby/range', false, Nodes::Mapping::BLOCK)
        ['begin', o.begin, 'end', o.end, 'excl', o.exclude_end?].each do |m|
          accept m
        end
        @emitter.end_mapping
      end

      def visit_Hash o
        tag      = o.class == ::Hash ? nil : "!ruby/hash:#{o.class}"
        implicit = !tag

        register(o, @emitter.start_mapping(nil, tag, implicit, Psych::Nodes::Mapping::BLOCK))

        o.each do |k,v|
          accept k
          accept v
        end

        @emitter.end_mapping
      end

      def visit_Psych_Set o
        register(o, @emitter.start_mapping(nil, '!set', false, Psych::Nodes::Mapping::BLOCK))

        o.each do |k,v|
          accept k
          accept v
        end

        @emitter.end_mapping
      end

      def visit_Array o
        if o.class == ::Array
          register o, @emitter.start_sequence(nil, nil, true, Nodes::Sequence::BLOCK)
          o.each { |c| accept c }
          @emitter.end_sequence
        else
          visit_array_subclass o
        end
      end

      def visit_NilClass o
        @emitter.scalar('', nil, 'tag:yaml.org,2002:null', true, false, Nodes::Scalar::ANY)
      end

      def visit_Symbol o
        @emitter.scalar ":#{o}", nil, nil, true, false, Nodes::Scalar::ANY
      end

      private
      def visit_array_subclass o
        tag = "!ruby/array:#{o.class}"
        if o.instance_variables.empty?
          node = @emitter.start_sequence(nil, tag, false, Nodes::Sequence::BLOCK)
          register o, node
          o.each { |c| accept c }
          @emitter.end_sequence
        else
          node = @emitter.start_mapping(nil, tag, false, Nodes::Sequence::BLOCK)
          register o, node

          # Dump the internal list
          accept 'internal'
          @emitter.start_sequence(nil, nil, true, Nodes::Sequence::BLOCK)
          o.each { |c| accept c }
          @emitter.end_sequence

          # Dump the ivars
          accept 'ivars'
          @emitter.start_mapping(nil, nil, true, Nodes::Sequence::BLOCK)
          o.instance_variables.each do |ivar|
            accept ivar
            accept o.instance_variable_get ivar
          end
          @emitter.end_mapping

          @emitter.end_mapping
        end
      end

      def dump_list o
      end

      # '%:z' was no defined until 1.9.3
      if RUBY_VERSION < '1.9.3'
        def format_time time
          formatted = time.strftime("%Y-%m-%d %H:%M:%S.%9N")

          if time.utc?
            formatted += " Z"
          else
            zone = time.strftime('%z')
            formatted += " #{zone[0,3]}:#{zone[3,5]}"
          end

          formatted
        end
      else
        def format_time time
          if time.utc?
            time.strftime("%Y-%m-%d %H:%M:%S.%9N Z")
          else
            time.strftime("%Y-%m-%d %H:%M:%S.%9N %:z")
          end
        end
      end

      # FIXME: remove this method once "to_yaml_properties" is removed
      def find_ivars target
        begin
          loc = target.method(:to_yaml_properties).source_location.first
          unless loc.start_with?(Psych::DEPRECATED) || loc.end_with?('rubytypes.rb')
            if $VERBOSE
              warn "#{loc}: to_yaml_properties is deprecated, please implement \"encode_with(coder)\""
            end
            return target.to_yaml_properties
          end
        rescue
          # public_method or source_location might be overridden,
          # and it's OK to skip it since it's only to emit a warning.
        end

        target.instance_variables
      end

      def register target, yaml_obj
        @st[target.object_id] = yaml_obj
        yaml_obj
      end

      def dump_coder o
        tag = Psych.dump_tags[o.class]
        unless tag
          klass = o.class == Object ? nil : o.class.name
          tag   = ['!ruby/object', klass].compact.join(':')
        end

        c = Psych::Coder.new(tag)
        o.encode_with(c)
        emit_coder c
      end

      def emit_coder c
        case c.type
        when :scalar
          @emitter.scalar c.scalar, nil, c.tag, c.tag.nil?, false, Nodes::Scalar::ANY
        when :seq
          @emitter.start_sequence nil, c.tag, c.tag.nil?, Nodes::Sequence::BLOCK
          c.seq.each do |thing|
            accept thing
          end
          @emitter.end_sequence
        when :map
          @emitter.start_mapping nil, c.tag, c.implicit, c.style
          c.map.each do |k,v|
            @emitter.scalar k, nil, nil, true, false, Nodes::Scalar::ANY
            accept v
          end
          @emitter.end_mapping
        when :object
          accept c.object
        end
      end

      def dump_ivars target
        ivars = find_ivars target

        ivars.each do |iv|
          @emitter.scalar("#{iv.to_s.sub(/^@/, '')}", nil, nil, true, false, Nodes::Scalar::ANY)
          accept target.instance_variable_get(iv)
        end
      end
    end
  end
end
