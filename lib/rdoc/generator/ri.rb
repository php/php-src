require 'rdoc/generator'
require 'rdoc/ri'

##
# Generates ri data files

class RDoc::Generator::RI

  RDoc::RDoc.add_generator self

  ##
  # Description of this generator

  DESCRIPTION = 'creates ri data files'

  ##
  # Set up a new ri generator

  def initialize options #:not-new:
    @options     = options
    @old_siginfo = nil
    @current     = nil

    @store          = RDoc::RI::Store.new '.'
    @store.dry_run  = @options.dry_run
    @store.encoding = @options.encoding if @options.respond_to? :encoding
  end

  ##
  # Build the initial indices and output objects based on an array of TopLevel
  # objects containing the extracted information.

  def generate top_levels
    install_siginfo_handler

    @store.load_cache

    RDoc::TopLevel.all_classes_and_modules.each do |klass|
      @current = "#{klass.class}: #{klass.full_name}"

      @store.save_class klass

      klass.each_method do |method|
        @current = "#{method.class}: #{method.full_name}"
        @store.save_method klass, method
      end

      klass.each_attribute do |attribute|
        @store.save_method klass, attribute
      end
    end

    @current = 'saving cache'

    @store.save_cache

  ensure
    @current = nil

    remove_siginfo_handler
  end

  ##
  # Installs a siginfo handler that prints the current filename.

  def install_siginfo_handler
    return unless Signal.list.key? 'INFO'

    @old_siginfo = trap 'INFO' do
      puts @current if @current
    end
  end

  ##
  # Removes a siginfo handler and replaces the previous

  def remove_siginfo_handler
    return unless Signal.list.key? 'INFO'

    handler = @old_siginfo || 'DEFAULT'

    trap 'INFO', handler
  end

end

