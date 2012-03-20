module RSS
  module Utils
    module_function

    # Convert a name_with_underscores to CamelCase.
    def to_class_name(name)
      name.split(/[_\-]/).collect do |part|
        "#{part[0, 1].upcase}#{part[1..-1]}"
      end.join("")
    end

    def get_file_and_line_from_caller(i=0)
      file, line, = caller[i].split(':')
      line = line.to_i
      line += 1 if i.zero?
      [file, line]
    end

    # escape '&', '"', '<' and '>' for use in HTML.
    def html_escape(s)
      s.to_s.gsub(/&/, "&amp;").gsub(/\"/, "&quot;").gsub(/>/, "&gt;").gsub(/</, "&lt;")
    end
    alias h html_escape

    # If +value+ is an instance of class +klass+, return it, else
    # create a new instance of +klass+ with value +value+.
    def new_with_value_if_need(klass, value)
      if value.is_a?(klass)
        value
      else
        klass.new(value)
      end
    end

    def element_initialize_arguments?(args)
      [true, false].include?(args[0]) and args[1].is_a?(Hash)
    end

    module YesCleanOther
      module_function
      def parse(value)
        if [true, false, nil].include?(value)
          value
        else
          case value.to_s
          when /\Ayes\z/i
            true
          when /\Aclean\z/i
            false
          else
            nil
          end
        end
      end
    end

    module YesOther
      module_function
      def parse(value)
        if [true, false].include?(value)
          value
        else
          /\Ayes\z/i.match(value.to_s) ? true : false
        end
      end
    end

    module CSV
      module_function
      def parse(value, &block)
        if value.is_a?(String)
          value = value.strip.split(/\s*,\s*/)
          value = value.collect(&block) if block_given?
          value
        else
          value
        end
      end
    end

    module InheritedReader
      def inherited_reader(constant_name)
        base_class = inherited_base
        result = base_class.const_get(constant_name)
        found_base_class = false
        ancestors.reverse_each do |klass|
          if found_base_class
            if klass.const_defined?(constant_name)
              result = yield(result, klass.const_get(constant_name))
            end
          else
            found_base_class = klass == base_class
          end
        end
        result
      end

      def inherited_array_reader(constant_name)
        inherited_reader(constant_name) do |result, current|
          current + result
        end
      end

      def inherited_hash_reader(constant_name)
        inherited_reader(constant_name) do |result, current|
          result.merge(current)
        end
      end
    end
  end
end
