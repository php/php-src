module Rake
  # ##########################################################################
  # Mixin for creating easily cloned objects.
  #
  module Cloneable
    # Clone an object by making a new object and setting all the instance
    # variables to the same values.
    def dup
      sibling = self.class.new
      instance_variables.each do |ivar|
        value = self.instance_variable_get(ivar)
        new_value = value.clone rescue value
        sibling.instance_variable_set(ivar, new_value)
      end
      sibling.taint if tainted?
      sibling
    end

    def clone
      sibling = dup
      sibling.freeze if frozen?
      sibling
    end
  end
end
