# The following classes used to be in the top level namespace.
# Loading this file enables compatibility with older Rakefile that
# referenced Task from the top level.

warn "WARNING: Classic namespaces are deprecated and will be removed from future versions of Rake."
# :stopdoc:
Task = Rake::Task
FileTask = Rake::FileTask
FileCreationTask = Rake::FileCreationTask
RakeApp = Rake::Application
# :startdoc:
