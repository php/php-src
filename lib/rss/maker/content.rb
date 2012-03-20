require 'rss/content'
require 'rss/maker/1.0'
require 'rss/maker/2.0'

module RSS
  module Maker
    module ContentModel
      def self.append_features(klass)
        super

        ::RSS::ContentModel::ELEMENTS.each do |name|
          klass.def_other_element(name)
        end
      end
    end

    class ItemsBase
      class ItemBase; include ContentModel; end
    end
  end
end
