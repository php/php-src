require 'rake/ext/core'
require 'rake/task'
require 'rake/file_task'
require 'rake/file_creation_task'
require 'rake/application'
require 'rake/task_manager'

######################################################################
# Rake extensions to Module.
#
class Module

  # Rename the original handler to make it available.
  alias :rake_original_const_missing :const_missing

  # Check for deprecated uses of top level (i.e. in Object) uses of
  # Rake class names.  If someone tries to reference the constant
  # name, display a warning and return the proper object.  Using the
  # --classic-namespace command line option will define these
  # constants in Object and avoid this handler.
  def const_missing(const_name)
    case const_name
    when :Task
      Rake.application.const_warning(const_name)
      Rake::Task
    when :FileTask
      Rake.application.const_warning(const_name)
      Rake::FileTask
    when :FileCreationTask
      Rake.application.const_warning(const_name)
      Rake::FileCreationTask
    when :RakeApp
      Rake.application.const_warning(const_name)
      Rake::Application
    else
      rake_original_const_missing(const_name)
    end
  end
end
