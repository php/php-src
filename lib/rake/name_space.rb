module Rake

  # The NameSpace class will lookup task names in the the scope
  # defined by a +namespace+ command.
  #
  class NameSpace

    # Create a namespace lookup object using the given task manager
    # and the list of scopes.
    def initialize(task_manager, scope_list)
      @task_manager = task_manager
      @scope = scope_list.dup
    end

    # Lookup a task named +name+ in the namespace.
    def [](name)
      @task_manager.lookup(name, @scope)
    end

    # Return the list of tasks defined in this and nested namespaces.
    def tasks
      @task_manager.tasks_in_scope(@scope)
    end
  end
end
