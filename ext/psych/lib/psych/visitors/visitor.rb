module Psych
  module Visitors
    class Visitor
      def accept target
        visit target
      end

      private

      DISPATCH = Hash.new do |hash, klass|
        hash[klass] = "visit_#{klass.name.gsub('::', '_')}"
      end

      def visit target
        send DISPATCH[target.class], target
      end
    end
  end
end
