require "rss/parser"

module RSS

  module RSS10
    NSPOOL = {}
    ELEMENTS = []

    def self.append_features(klass)
      super

      klass.install_must_call_validator('', ::RSS::URI)
    end

  end

  class RDF < Element

    include RSS10
    include RootElementMixin

    class << self

      def required_uri
        URI
      end

    end

    @tag_name = 'RDF'

    PREFIX = 'rdf'
    URI = "http://www.w3.org/1999/02/22-rdf-syntax-ns#"

    install_ns('', ::RSS::URI)
    install_ns(PREFIX, URI)

    [
      ["channel", nil],
      ["image", "?"],
      ["item", "+", :children],
      ["textinput", "?"],
    ].each do |tag, occurs, type|
      type ||= :child
      __send__("install_have_#{type}_element", tag, ::RSS::URI, occurs)
    end

    alias_method(:rss_version, :feed_version)
    def initialize(version=nil, encoding=nil, standalone=nil)
      super('1.0', version, encoding, standalone)
      @feed_type = "rss"
    end

    def full_name
      tag_name_with_prefix(PREFIX)
    end

    class Li < Element

      include RSS10

      class << self
        def required_uri
          URI
        end
      end

      [
        ["resource", [URI, ""], true]
      ].each do |name, uri, required|
        install_get_attribute(name, uri, required)
      end

      def initialize(*args)
        if Utils.element_initialize_arguments?(args)
          super
        else
          super()
          self.resource = args[0]
        end
      end

      def full_name
        tag_name_with_prefix(PREFIX)
      end
    end

    class Seq < Element

      include RSS10

      Li = ::RSS::RDF::Li

      class << self
        def required_uri
          URI
        end
      end

      @tag_name = 'Seq'

      install_have_children_element("li", URI, "*")
      install_must_call_validator('rdf', ::RSS::RDF::URI)

      def initialize(*args)
        if Utils.element_initialize_arguments?(args)
          super
        else
          super()
          @li = args[0] if args[0]
        end
      end

      def full_name
        tag_name_with_prefix(PREFIX)
      end

      def setup_maker(target)
        lis.each do |li|
          target << li.resource
        end
      end
    end

    class Bag < Element

      include RSS10

      Li = ::RSS::RDF::Li

      class << self
        def required_uri
          URI
        end
      end

      @tag_name = 'Bag'

      install_have_children_element("li", URI, "*")
      install_must_call_validator('rdf', URI)

      def initialize(*args)
        if Utils.element_initialize_arguments?(args)
          super
        else
          super()
          @li = args[0] if args[0]
        end
      end

      def full_name
        tag_name_with_prefix(PREFIX)
      end

      def setup_maker(target)
        lis.each do |li|
          target << li.resource
        end
      end
    end

    class Channel < Element

      include RSS10

      class << self

        def required_uri
          ::RSS::URI
        end

      end

      [
        ["about", URI, true]
      ].each do |name, uri, required|
        install_get_attribute(name, uri, required, nil, nil,
                              "#{PREFIX}:#{name}")
      end

      [
        ['title', nil, :text],
        ['link', nil, :text],
        ['description', nil, :text],
        ['image', '?', :have_child],
        ['items', nil, :have_child],
        ['textinput', '?', :have_child],
      ].each do |tag, occurs, type|
        __send__("install_#{type}_element", tag, ::RSS::URI, occurs)
      end

      def initialize(*args)
        if Utils.element_initialize_arguments?(args)
          super
        else
          super()
          self.about = args[0]
        end
      end

      private
      def maker_target(maker)
        maker.channel
      end

      def setup_maker_attributes(channel)
        channel.about = about
      end

      class Image < Element

        include RSS10

        class << self

          def required_uri
            ::RSS::URI
          end

        end

        [
          ["resource", URI, true]
        ].each do |name, uri, required|
          install_get_attribute(name, uri, required, nil, nil,
                                "#{PREFIX}:#{name}")
        end

        def initialize(*args)
          if Utils.element_initialize_arguments?(args)
            super
          else
            super()
            self.resource = args[0]
          end
        end
      end

      class Textinput < Element

        include RSS10

        class << self

          def required_uri
            ::RSS::URI
          end

        end

        [
          ["resource", URI, true]
        ].each do |name, uri, required|
          install_get_attribute(name, uri, required, nil, nil,
                                "#{PREFIX}:#{name}")
        end

        def initialize(*args)
          if Utils.element_initialize_arguments?(args)
            super
          else
            super()
            self.resource = args[0]
          end
        end
      end

      class Items < Element

        include RSS10

        Seq = ::RSS::RDF::Seq

        class << self

          def required_uri
            ::RSS::URI
          end

        end

        install_have_child_element("Seq", URI, nil)
        install_must_call_validator('rdf', URI)

        def initialize(*args)
          if Utils.element_initialize_arguments?(args)
            super
          else
            super()
            self.Seq = args[0]
          end
          self.Seq ||= Seq.new
        end

        def resources
          if @Seq
            @Seq.lis.collect do |li|
              li.resource
            end
          else
            []
          end
        end
      end
    end

    class Image < Element

      include RSS10

      class << self

        def required_uri
          ::RSS::URI
        end

      end

      [
        ["about", URI, true]
      ].each do |name, uri, required|
        install_get_attribute(name, uri, required, nil, nil,
                              "#{PREFIX}:#{name}")
      end

      %w(title url link).each do |name|
        install_text_element(name, ::RSS::URI, nil)
      end

      def initialize(*args)
        if Utils.element_initialize_arguments?(args)
          super
        else
          super()
          self.about = args[0]
        end
      end

      private
      def maker_target(maker)
        maker.image
      end
    end

    class Item < Element

      include RSS10

      class << self

        def required_uri
          ::RSS::URI
        end

      end


      [
        ["about", URI, true]
      ].each do |name, uri, required|
        install_get_attribute(name, uri, required, nil, nil,
                              "#{PREFIX}:#{name}")
      end

      [
        ["title", nil],
        ["link", nil],
        ["description", "?"],
      ].each do |tag, occurs|
        install_text_element(tag, ::RSS::URI, occurs)
      end

      def initialize(*args)
        if Utils.element_initialize_arguments?(args)
          super
        else
          super()
          self.about = args[0]
        end
      end

      private
      def maker_target(items)
        if items.respond_to?("items")
          # For backward compatibility
          items = items.items
        end
        items.new_item
      end
    end

    class Textinput < Element

      include RSS10

      class << self

        def required_uri
          ::RSS::URI
        end

      end

      [
        ["about", URI, true]
      ].each do |name, uri, required|
        install_get_attribute(name, uri, required, nil, nil,
                              "#{PREFIX}:#{name}")
      end

      %w(title description name link).each do |name|
        install_text_element(name, ::RSS::URI, nil)
      end

      def initialize(*args)
        if Utils.element_initialize_arguments?(args)
          super
        else
          super()
          self.about = args[0]
        end
      end

      private
      def maker_target(maker)
        maker.textinput
      end
    end

  end

  RSS10::ELEMENTS.each do |name|
    BaseListener.install_get_text_element(URI, name, name)
  end

  module ListenerMixin
    private
    def initial_start_RDF(tag_name, prefix, attrs, ns)
      check_ns(tag_name, prefix, ns, RDF::URI, false)

      @rss = RDF.new(@version, @encoding, @standalone)
      @rss.do_validate = @do_validate
      @rss.xml_stylesheets = @xml_stylesheets
      @last_element = @rss
      pr = Proc.new do |text, tags|
        @rss.validate_for_stream(tags, @ignore_unknown_element) if @do_validate
      end
      @proc_stack.push(pr)
    end
  end

end
