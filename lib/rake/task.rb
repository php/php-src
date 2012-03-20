require 'rake/invocation_exception_mixin'

module Rake

  # #########################################################################
  # A Task is the basic unit of work in a Rakefile.  Tasks have associated
  # actions (possibly more than one) and a list of prerequisites.  When
  # invoked, a task will first ensure that all of its prerequisites have an
  # opportunity to run and then it will execute its own actions.
  #
  # Tasks are not usually created directly using the new method, but rather
  # use the +file+ and +task+ convenience methods.
  #
  class Task
    # List of prerequisites for a task.
    attr_reader :prerequisites

    # List of actions attached to a task.
    attr_reader :actions

    # Application owning this task.
    attr_accessor :application

    # Comment for this task.  Restricted to a single line of no more than 50
    # characters.
    attr_reader :comment

    # Full text of the (possibly multi-line) comment.
    attr_reader :full_comment

    # Array of nested namespaces names used for task lookup by this task.
    attr_reader :scope

    # File/Line locations of each of the task definitions for this
    # task (only valid if the task was defined with the detect
    # location option set).
    attr_reader :locations

    # Return task name
    def to_s
      name
    end

    def inspect
      "<#{self.class} #{name} => [#{prerequisites.join(', ')}]>"
    end

    # List of sources for task.
    attr_writer :sources
    def sources
      @sources ||= []
    end

    # List of prerequisite tasks
    def prerequisite_tasks
      prerequisites.collect { |pre| lookup_prerequisite(pre) }
    end

    def lookup_prerequisite(prerequisite_name)
      application[prerequisite_name, @scope]
    end
    private :lookup_prerequisite

    # First source from a rule (nil if no sources)
    def source
      @sources.first if defined?(@sources)
    end

    # Create a task named +task_name+ with no actions or prerequisites. Use
    # +enhance+ to add actions and prerequisites.
    def initialize(task_name, app)
      @name = task_name.to_s
      @prerequisites = []
      @actions = []
      @already_invoked = false
      @full_comment = nil
      @comment = nil
      @lock = Monitor.new
      @application = app
      @scope = app.current_scope
      @arg_names = nil
      @locations = []
    end

    # Enhance a task with prerequisites or actions.  Returns self.
    def enhance(deps=nil, &block)
      @prerequisites |= deps if deps
      @actions << block if block_given?
      self
    end

    # Name of the task, including any namespace qualifiers.
    def name
      @name.to_s
    end

    # Name of task with argument list description.
    def name_with_args # :nodoc:
      if arg_description
        "#{name}#{arg_description}"
      else
        name
      end
    end

    # Argument description (nil if none).
    def arg_description # :nodoc:
      @arg_names ? "[#{(arg_names || []).join(',')}]" : nil
    end

    # Name of arguments for this task.
    def arg_names
      @arg_names || []
    end

    # Reenable the task, allowing its tasks to be executed if the task
    # is invoked again.
    def reenable
      @already_invoked = false
    end

    # Clear the existing prerequisites and actions of a rake task.
    def clear
      clear_prerequisites
      clear_actions
      self
    end

    # Clear the existing prerequisites of a rake task.
    def clear_prerequisites
      prerequisites.clear
      self
    end

    # Clear the existing actions on a rake task.
    def clear_actions
      actions.clear
      self
    end

    # Invoke the task if it is needed.  Prerequisites are invoked first.
    def invoke(*args)
      task_args = TaskArguments.new(arg_names, args)
      invoke_with_call_chain(task_args, InvocationChain::EMPTY)
    end

    # Same as invoke, but explicitly pass a call chain to detect
    # circular dependencies.
    def invoke_with_call_chain(task_args, invocation_chain) # :nodoc:
      new_chain = InvocationChain.append(self, invocation_chain)
      @lock.synchronize do
        if application.options.trace
          $stderr.puts "** Invoke #{name} #{format_trace_flags}"
        end
        return if @already_invoked
        @already_invoked = true
        invoke_prerequisites(task_args, new_chain)
        execute(task_args) if needed?
      end
    rescue Exception => ex
      add_chain_to(ex, new_chain)
      raise ex
    end
    protected :invoke_with_call_chain

    def add_chain_to(exception, new_chain)
      exception.extend(InvocationExceptionMixin) unless exception.respond_to?(:chain)
      exception.chain = new_chain if exception.chain.nil?
    end
    private :add_chain_to

    # Invoke all the prerequisites of a task.
    def invoke_prerequisites(task_args, invocation_chain) # :nodoc:
      prerequisite_tasks.each { |prereq|
        prereq_args = task_args.new_scope(prereq.arg_names)
        prereq.invoke_with_call_chain(prereq_args, invocation_chain)
      }
    end

    # Format the trace flags for display.
    def format_trace_flags
      flags = []
      flags << "first_time" unless @already_invoked
      flags << "not_needed" unless needed?
      flags.empty? ? "" : "(" + flags.join(", ") + ")"
    end
    private :format_trace_flags

    # Execute the actions associated with this task.
    def execute(args=nil)
      args ||= EMPTY_TASK_ARGS
      if application.options.dryrun
        $stderr.puts "** Execute (dry run) #{name}"
        return
      end
      if application.options.trace
        $stderr.puts "** Execute #{name}"
      end
      application.enhance_with_matching_rule(name) if @actions.empty?
      @actions.each do |act|
        case act.arity
        when 1
          act.call(self)
        else
          act.call(self, args)
        end
      end
    end

    # Is this task needed?
    def needed?
      true
    end

    # Timestamp for this task.  Basic tasks return the current time for their
    # time stamp.  Other tasks can be more sophisticated.
    def timestamp
      prerequisite_tasks.collect { |pre| pre.timestamp }.max || Time.now
    end

    # Add a description to the task.  The description can consist of an option
    # argument list (enclosed brackets) and an optional comment.
    def add_description(description)
      return if ! description
      comment = description.strip
      add_comment(comment) if comment && ! comment.empty?
    end

    # Writing to the comment attribute is the same as adding a description.
    def comment=(description)
      add_description(description)
    end

    # Add a comment to the task.  If a comment already exists, separate
    # the new comment with " / ".
    def add_comment(comment)
      if @full_comment
        @full_comment << " / "
      else
        @full_comment = ''
      end
      @full_comment << comment
      if @full_comment =~ /\A([^.]+?\.)( |$)/
        @comment = $1
      else
        @comment = @full_comment
      end
    end
    private :add_comment

    # Set the names of the arguments for this task. +args+ should be
    # an array of symbols, one for each argument name.
    def set_arg_names(args)
      @arg_names = args.map { |a| a.to_sym }
    end

    # Return a string describing the internal state of a task.  Useful for
    # debugging.
    def investigation
      result = "------------------------------\n"
      result << "Investigating #{name}\n"
      result << "class: #{self.class}\n"
      result <<  "task needed: #{needed?}\n"
      result <<  "timestamp: #{timestamp}\n"
      result << "pre-requisites: \n"
      prereqs = prerequisite_tasks
      prereqs.sort! {|a,b| a.timestamp <=> b.timestamp}
      prereqs.each do |p|
        result << "--#{p.name} (#{p.timestamp})\n"
      end
      latest_prereq = prerequisite_tasks.collect { |pre| pre.timestamp }.max
      result <<  "latest-prerequisite time: #{latest_prereq}\n"
      result << "................................\n\n"
      return result
    end

    # ----------------------------------------------------------------
    # Rake Module Methods
    #
    class << self

      # Clear the task list.  This cause rake to immediately forget all the
      # tasks that have been assigned.  (Normally used in the unit tests.)
      def clear
        Rake.application.clear
      end

      # List of all defined tasks.
      def tasks
        Rake.application.tasks
      end

      # Return a task with the given name.  If the task is not currently
      # known, try to synthesize one from the defined rules.  If no rules are
      # found, but an existing file matches the task name, assume it is a file
      # task with no dependencies or actions.
      def [](task_name)
        Rake.application[task_name]
      end

      # TRUE if the task name is already defined.
      def task_defined?(task_name)
        Rake.application.lookup(task_name) != nil
      end

      # Define a task given +args+ and an option block.  If a rule with the
      # given name already exists, the prerequisites and actions are added to
      # the existing task.  Returns the defined task.
      def define_task(*args, &block)
        Rake.application.define_task(self, *args, &block)
      end

      # Define a rule for synthesizing tasks.
      def create_rule(*args, &block)
        Rake.application.create_rule(*args, &block)
      end

      # Apply the scope to the task name according to the rules for
      # this kind of task.  Generic tasks will accept the scope as
      # part of the name.
      def scope_name(scope, task_name)
        (scope + [task_name]).join(':')
      end

    end # class << Rake::Task
  end # class Rake::Task
end
