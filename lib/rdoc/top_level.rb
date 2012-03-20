require 'rdoc/context'

##
# A TopLevel context is a representation of the contents of a single file

class RDoc::TopLevel < RDoc::Context

  ##
  # This TopLevel's File::Stat struct

  attr_accessor :file_stat

  ##
  # Relative name of this file

  attr_accessor :relative_name

  ##
  # Absolute name of this file

  attr_accessor :absolute_name

  ##
  # All the classes or modules that were declared in
  # this file. These are assigned to either +#classes_hash+
  # or +#modules_hash+ once we know what they really are.

  attr_reader :classes_or_modules

  attr_accessor :diagram # :nodoc:

  ##
  # The parser that processed this file

  attr_accessor :parser

  ##
  # Returns all classes discovered by RDoc

  def self.all_classes
    @all_classes_hash.values
  end

  ##
  # Returns all classes and modules discovered by RDoc

  def self.all_classes_and_modules
    @all_classes_hash.values + @all_modules_hash.values
  end

  ##
  # Hash of all classes known to RDoc

  def self.all_classes_hash
    @all_classes_hash
  end

  ##
  # All TopLevels known to RDoc

  def self.all_files
    @all_files_hash.values
  end

  ##
  # Hash of all files known to RDoc

  def self.all_files_hash
    @all_files_hash
  end

  ##
  # Returns all modules discovered by RDoc

  def self.all_modules
    all_modules_hash.values
  end

  ##
  # Hash of all modules known to RDoc

  def self.all_modules_hash
    @all_modules_hash
  end

  ##
  # Prepares the RDoc code object tree for use by a generator.
  #
  # It finds unique classes/modules defined, and replaces classes/modules that
  # are aliases for another one by a copy with RDoc::ClassModule#is_alias_for
  # set.
  #
  # It updates the RDoc::ClassModule#constant_aliases attribute of "real"
  # classes or modules.
  #
  # It also completely removes the classes and modules that should be removed
  # from the documentation and the methods that have a visibility below
  # +min_visibility+, which is the <tt>--visibility</tt> option.
  #
  # See also RDoc::Context#remove_from_documentation?

  def self.complete min_visibility
    fix_basic_object_inheritance

    # cache included modules before they are removed from the documentation
    all_classes_and_modules.each { |cm| cm.ancestors }

    remove_nodoc @all_classes_hash
    remove_nodoc @all_modules_hash

    @unique_classes = find_unique @all_classes_hash
    @unique_modules = find_unique @all_modules_hash

    unique_classes_and_modules.each do |cm|
      cm.complete min_visibility
    end

    @all_files_hash.each_key do |file_name|
      tl = @all_files_hash[file_name]

      unless RDoc::Parser::Simple === tl.parser then
        tl.modules_hash.clear
        tl.classes_hash.clear

        tl.classes_or_modules.each do |cm|
          name = cm.full_name
          if cm.type == 'class' then
            tl.classes_hash[name] = cm if @all_classes_hash[name]
          else
            tl.modules_hash[name] = cm if @all_modules_hash[name]
          end
        end
      end
    end
  end

  ##
  # Finds the class with +name+ in all discovered classes

  def self.find_class_named(name)
    @all_classes_hash[name]
  end

  ##
  # Finds the class with +name+ starting in namespace +from+

  def self.find_class_named_from name, from
    from = find_class_named from unless RDoc::Context === from

    until RDoc::TopLevel === from do
      return nil unless from

      klass = from.find_class_named name
      return klass if klass

      from = from.parent
    end

    find_class_named name
  end

  ##
  # Finds the class or module with +name+

  def self.find_class_or_module(name)
    name = $' if name =~ /^::/
    RDoc::TopLevel.classes_hash[name] || RDoc::TopLevel.modules_hash[name]
  end

  ##
  # Finds the file with +name+ in all discovered files

  def self.find_file_named(name)
    @all_files_hash[name]
  end

  ##
  # Finds the module with +name+ in all discovered modules

  def self.find_module_named(name)
    modules_hash[name]
  end

  ##
  # Finds unique classes/modules defined in +all_hash+,
  # and returns them as an array. Performs the alias
  # updates in +all_hash+: see ::complete.
  #--
  # TODO  aliases should be registered by Context#add_module_alias

  def self.find_unique(all_hash)
    unique = []

    all_hash.each_pair do |full_name, cm|
      unique << cm if full_name == cm.full_name
    end

    unique
  end

  ##
  # Fixes the erroneous <tt>BasicObject < Object</tt> in 1.9.
  #
  # Because we assumed all classes without a stated superclass
  # inherit from Object, we have the above wrong inheritance.
  #
  # We fix BasicObject right away if we are running in a Ruby
  # version >= 1.9. If not, we may be documenting 1.9 source
  # while running under 1.8: we search the files of BasicObject
  # for "object.c", and fix the inheritance if we find it.

  def self.fix_basic_object_inheritance
    basic = all_classes_hash['BasicObject']
    return unless basic
    if RUBY_VERSION >= '1.9'
      basic.superclass = nil
    elsif basic.in_files.any? { |f| File.basename(f.full_name) == 'object.c' }
      basic.superclass = nil
    end
  end

  ##
  # Creates a new RDoc::TopLevel with +file_name+ only if one with the same
  # name does not exist in all_files.

  def self.new file_name
    if top_level = @all_files_hash[file_name] then
      top_level
    else
      top_level = super
      @all_files_hash[file_name] = top_level
      top_level
    end
  end

  ##
  # Removes from +all_hash+ the contexts that are nodoc or have no content.
  #
  # See RDoc::Context#remove_from_documentation?

  def self.remove_nodoc(all_hash)
    all_hash.keys.each do |name|
      context = all_hash[name]
      all_hash.delete(name) if context.remove_from_documentation?
    end
  end

  ##
  # Empties RDoc of stored class, module and file information

  def self.reset
    @all_classes_hash = {}
    @all_modules_hash = {}
    @all_files_hash   = {}
  end

  ##
  # Returns the unique classes discovered by RDoc.
  #
  # ::complete must have been called prior to using this method.

  def self.unique_classes
    @unique_classes
  end

  ##
  # Returns the unique classes and modules discovered by RDoc.
  # ::complete must have been called prior to using this method.

  def self.unique_classes_and_modules
    @unique_classes + @unique_modules
  end

  ##
  # Returns the unique modules discovered by RDoc.
  # ::complete must have been called prior to using this method.

  def self.unique_modules
    @unique_modules
  end

  class << self
    alias classes      all_classes
    alias classes_hash all_classes_hash

    alias files        all_files
    alias files_hash   all_files_hash

    alias modules      all_modules
    alias modules_hash all_modules_hash
  end

  reset

  ##
  # Creates a new TopLevel for +file_name+

  def initialize(file_name)
    super()
    @name = nil
    @relative_name = file_name
    @absolute_name = file_name
    @file_stat     = File.stat(file_name) rescue nil # HACK for testing
    @diagram       = nil
    @parser        = nil

    @classes_or_modules = []

    RDoc::TopLevel.files_hash[file_name] = self
  end

  ##
  # An RDoc::TopLevel is equal to another with the same absolute_name

  def == other
    other.class === self and @absolute_name == other.absolute_name
  end

  alias eql? ==

  ##
  # Adds +an_alias+ to +Object+ instead of +self+.

  def add_alias(an_alias)
    object_class.record_location self
    return an_alias unless @document_self
    object_class.add_alias an_alias
  end

  ##
  # Adds +constant+ to +Object+ instead of +self+.

  def add_constant(constant)
    object_class.record_location self
    return constant unless @document_self
    object_class.add_constant constant
  end

  ##
  # Adds +include+ to +Object+ instead of +self+.

  def add_include(include)
    object_class.record_location self
    return include unless @document_self
    object_class.add_include include
  end

  ##
  # Adds +method+ to +Object+ instead of +self+.

  def add_method(method)
    object_class.record_location self
    return method unless @document_self
    object_class.add_method method
  end

  ##
  # Adds class or module +mod+. Used in the building phase
  # by the ruby parser.

  def add_to_classes_or_modules mod
    @classes_or_modules << mod
  end

  ##
  # Base name of this file

  def base_name
    File.basename @absolute_name
  end

  alias name base_name

  ##
  # See RDoc::TopLevel::find_class_or_module
  #--
  # TODO Why do we search through all classes/modules found, not just the
  #       ones of this instance?

  def find_class_or_module name
    RDoc::TopLevel.find_class_or_module name
  end

  ##
  # Finds a class or module named +symbol+

  def find_local_symbol(symbol)
    find_class_or_module(symbol) || super
  end

  ##
  # Finds a module or class with +name+

  def find_module_named(name)
    find_class_or_module(name)
  end

  ##
  # Returns the relative name of this file

  def full_name
    @relative_name
  end

  ##
  # An RDoc::TopLevel has the same hash as another with the same
  # absolute_name

  def hash
    @absolute_name.hash
  end

  ##
  # URL for this with a +prefix+

  def http_url(prefix)
    path = [prefix, @relative_name.tr('.', '_')]

    File.join(*path.compact) + '.html'
  end

  def inspect # :nodoc:
    "#<%s:0x%x %p modules: %p classes: %p>" % [
      self.class, object_id,
      base_name,
      @modules.map { |n,m| m },
      @classes.map { |n,c| c }
    ]
  end

  ##
  # Time this file was last modified, if known

  def last_modified
    @file_stat ? file_stat.mtime : nil
  end

  ##
  # Returns the NormalClass "Object", creating it if not found.
  #
  # Records +self+ as a location in "Object".

  def object_class
    @object_class ||= begin
      oc = self.class.find_class_named('Object') || add_class(RDoc::NormalClass, 'Object')
      oc.record_location self
      oc
    end
  end

  ##
  # Path to this file

  def path
    http_url RDoc::RDoc.current.generator.file_dir
  end

  def pretty_print q # :nodoc:
    q.group 2, "[#{self.class}: ", "]" do
      q.text "base name: #{base_name.inspect}"
      q.breakable

      items = @modules.map { |n,m| m }
      items.push(*@modules.map { |n,c| c })
      q.seplist items do |mod| q.pp mod end
    end
  end

  def to_s # :nodoc:
    "file #{full_name}"
  end

end

