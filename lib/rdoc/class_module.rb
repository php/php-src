require 'rdoc/context'

##
# ClassModule is the base class for objects representing either a class or a
# module.

class RDoc::ClassModule < RDoc::Context

  ##
  # 1::
  #   RDoc 3.7
  #   * Added visibility, singleton and file to attributes
  #   * Added file to constants
  #   * Added file to includes
  #   * Added file to methods

  MARSHAL_VERSION = 1 # :nodoc:

  ##
  # Constants that are aliases for this class or module

  attr_accessor :constant_aliases

  ##
  # Comment and the location it came from.  Use #add_comment to add comments

  attr_reader :comment_location

  attr_accessor :diagram # :nodoc:

  ##
  # Class or module this constant is an alias for

  attr_accessor :is_alias_for

  ##
  # Return a RDoc::ClassModule of class +class_type+ that is a copy
  # of module +module+. Used to promote modules to classes.
  #--
  # TODO move to RDoc::NormalClass (I think)

  def self.from_module class_type, mod
    klass = class_type.new mod.name

    mod.comment_location.each do |comment, location|
      klass.add_comment comment, location
    end

    klass.parent = mod.parent
    klass.section = mod.section
    klass.viewer = mod.viewer

    klass.attributes.concat mod.attributes
    klass.method_list.concat mod.method_list
    klass.aliases.concat mod.aliases
    klass.external_aliases.concat mod.external_aliases
    klass.constants.concat mod.constants
    klass.includes.concat mod.includes

    klass.methods_hash.update mod.methods_hash
    klass.constants_hash.update mod.constants_hash

    klass.current_section = mod.current_section
    klass.in_files.concat mod.in_files
    klass.sections.concat mod.sections
    klass.unmatched_alias_lists = mod.unmatched_alias_lists
    klass.current_section = mod.current_section
    klass.visibility = mod.visibility

    klass.classes_hash.update mod.classes_hash
    klass.modules_hash.update mod.modules_hash
    klass.metadata.update mod.metadata

    klass.document_self = mod.received_nodoc ? nil : mod.document_self
    klass.document_children = mod.document_children
    klass.force_documentation = mod.force_documentation
    klass.done_documenting = mod.done_documenting

    # update the parent of all children

    (klass.attributes +
     klass.method_list +
     klass.aliases +
     klass.external_aliases +
     klass.constants +
     klass.includes +
     klass.classes +
     klass.modules).each do |obj|
      obj.parent = klass
      obj.full_name = nil
    end

    klass
  end

  ##
  # Creates a new ClassModule with +name+ with optional +superclass+
  #
  # This is a constructor for subclasses, and must never be called directly.

  def initialize(name, superclass = nil)
    @constant_aliases = []
    @diagram          = nil
    @is_alias_for     = nil
    @name             = name
    @superclass       = superclass
    @comment_location = [] # [[comment, location]]

    super()
  end

  ##
  # Adds +comment+ to this ClassModule's list of comments at +location+.  This
  # method is preferred over #comment= since it allows ri data to be updated
  # across multiple runs.

  def add_comment comment, location
    return if comment.empty? or not document_self

    original = comment

    comment = normalize_comment comment
    @comment_location << [comment, location]

    self.comment = original
  end

  ##
  # Ancestors list for this ClassModule: the list of included modules
  # (classes will add their superclass if any).
  #
  # Returns the included classes or modules, not the includes
  # themselves. The returned values are either String or
  # RDoc::NormalModule instances (see RDoc::Include#module).
  #
  # The values are returned in reverse order of their inclusion,
  # which is the order suitable for searching methods/attributes
  # in the ancestors. The superclass, if any, comes last.

  def ancestors
    includes.map { |i| i.module }.reverse
  end

  ##
  # Clears the comment. Used by the ruby parser.

  def clear_comment
    @comment = ''
  end

  ##
  # This method is deprecated, use #add_comment instead.
  #
  # Appends +comment+ to the current comment, but separated by a rule.  Works
  # more like <tt>+=</tt>.

  def comment= comment
    return if comment.empty?

    comment = normalize_comment comment
    comment = "#{@comment}\n---\n#{comment}" unless @comment.empty?

    super comment
  end

  ##
  # Prepares this ClassModule for use by a generator.
  #
  # See RDoc::TopLevel::complete

  def complete min_visibility
    update_aliases
    remove_nodoc_children
    update_includes
    remove_invisible min_visibility
  end

  ##
  # Iterates the ancestors of this class or module for which an
  # RDoc::ClassModule exists.

  def each_ancestor # :yields: module
    ancestors.each do |mod|
      next if String === mod
      yield mod
    end
  end

  ##
  # Looks for a symbol in the #ancestors. See Context#find_local_symbol.

  def find_ancestor_local_symbol symbol
    each_ancestor do |m|
      res = m.find_local_symbol(symbol)
      return res if res
    end

    nil
  end

  ##
  # Finds a class or module with +name+ in this namespace or its descendants

  def find_class_named name
    return self if full_name == name
    return self if @name == name

    @classes.values.find do |klass|
      next if klass == self
      klass.find_class_named name
    end
  end

  ##
  # Return the fully qualified name of this class or module

  def full_name
    @full_name ||= if RDoc::ClassModule === @parent then
                     "#{@parent.full_name}::#{@name}"
                   else
                     @name
                   end
  end

  ##
  # TODO: filter included items by #display?

  def marshal_dump # :nodoc:
    attrs = attributes.sort.map do |attr|
      [ attr.name, attr.rw,
        attr.visibility, attr.singleton, attr.file_name,
      ]
    end

    method_types = methods_by_type.map do |type, visibilities|
      visibilities = visibilities.map do |visibility, methods|
        method_names = methods.map do |method|
          [method.name, method.file_name]
        end

        [visibility, method_names.uniq]
      end

      [type, visibilities]
    end

    [ MARSHAL_VERSION,
      @name,
      full_name,
      @superclass,
      parse(@comment_location),
      attrs,
      constants.map do |const|
        [const.name, parse(const.comment), const.file_name]
      end,
      includes.map do |incl|
        [incl.name, parse(incl.comment), incl.file_name]
      end,
      method_types,
    ]
  end

  def marshal_load array # :nodoc:
    initialize_methods_etc
    @current_section   = nil
    @document_self     = true
    @done_documenting  = false
    @parent            = nil
    @temporary_section = nil
    @visibility        = nil

    @name       = array[1]
    @full_name  = array[2]
    @superclass = array[3]
    @comment    = array[4]

    @comment_location = if RDoc::Markup::Document === @comment.parts.first then
                          @comment
                        else
                          RDoc::Markup::Document.new @comment
                        end

    array[5].each do |name, rw, visibility, singleton, file|
      singleton  ||= false
      visibility ||= :public

      attr = RDoc::Attr.new nil, name, rw, nil, singleton

      add_attribute attr
      attr.visibility = visibility
      attr.record_location RDoc::TopLevel.new file
    end

    array[6].each do |name, comment, file|
      const = add_constant RDoc::Constant.new(name, nil, comment)
      const.record_location RDoc::TopLevel.new file
    end

    array[7].each do |name, comment, file|
      incl = add_include RDoc::Include.new(name, comment)
      incl.record_location RDoc::TopLevel.new file
    end

    array[8].each do |type, visibilities|
      visibilities.each do |visibility, methods|
        @visibility = visibility

        methods.each do |name, file|
          method = RDoc::AnyMethod.new nil, name
          method.singleton = true if type == 'class'
          method.record_location RDoc::TopLevel.new file
          add_method method
        end
      end
    end
  end

  ##
  # Merges +class_module+ into this ClassModule.
  #
  # The data in +class_module+ is preferred over the receiver.

  def merge class_module
    other_document = parse class_module.comment_location

    if other_document then
      document = parse @comment_location

      document = document.merge other_document

      @comment = @comment_location = document
    end

    cm = class_module
    other_files = cm.in_files

    merge_collections attributes, cm.attributes, other_files do |add, attr|
      if add then
        add_attribute attr
      else
        @attributes.delete attr
        @methods_hash.delete attr.pretty_name
      end
    end

    merge_collections constants, cm.constants, other_files do |add, const|
      if add then
        add_constant const
      else
        @constants.delete const
        @constants_hash.delete const.name
      end
    end

    merge_collections includes, cm.includes, other_files do |add, incl|
      if add then
        add_include incl
      else
        @includes.delete incl
      end
    end

    merge_collections method_list, cm.method_list, other_files do |add, meth|
      if add then
        add_method meth
      else
        @method_list.delete meth
        @methods_hash.delete meth.pretty_name
      end
    end

    self
  end

  ##
  # Merges collection +mine+ with +other+ preferring other.  +other_files+ is
  # used to help determine which items should be deleted.
  #
  # Yields whether the item should be added or removed (true or false) and the
  # item to be added or removed.
  #
  #   merge_collections things, other.things, other.in_files do |add, thing|
  #     if add then
  #       # add the thing
  #     else
  #       # remove the thing
  #     end
  #   end

  def merge_collections mine, other, other_files, &block # :nodoc:
    my_things    = mine. group_by { |thing| thing.file }
    other_things = other.group_by { |thing| thing.file }

    my_things.delete_if do |file, things|
      next false unless other_files.include? file

      things.each do |thing|
        yield false, thing
      end

      true
    end

    other_things.each do |file, things|
      my_things[file].each { |thing| yield false, thing } if
        my_things.include?(file)

      things.each do |thing|
        yield true, thing
      end
    end
  end

  ##
  # Does this object represent a module?

  def module?
    false
  end

  ##
  # Allows overriding the initial name.
  #
  # Used for modules and classes that are constant aliases.

  def name= new_name
    @name = new_name
  end

  ##
  # Parses +comment_location+ into an RDoc::Markup::Document composed of
  # multiple RDoc::Markup::Documents with their file set.

  def parse comment_location
    case comment_location
    when String then
      super
    when Array then
      docs = comment_location.map do |comment, location|
        doc = super comment
        doc.file = location.absolute_name
        doc
      end

      RDoc::Markup::Document.new(*docs)
    when RDoc::Markup::Document then
      return comment_location
    else
      raise ArgumentError, "unknown comment class #{comment_location.class}"
    end
  end

  ##
  # Path to this class or module

  def path
    http_url RDoc::RDoc.current.generator.class_dir
  end

  ##
  # Name to use to generate the url:
  # modules and classes that are aliases for another
  # module or class return the name of the latter.

  def name_for_path
    is_alias_for ? is_alias_for.full_name : full_name
  end

  ##
  # Returns the classes and modules that are not constants
  # aliasing another class or module. For use by formatters
  # only (caches its result).

  def non_aliases
    @non_aliases ||= classes_and_modules.reject { |cm| cm.is_alias_for }
  end

  ##
  # Updates the child modules or classes of class/module +parent+ by
  # deleting the ones that have been removed from the documentation.
  #
  # +parent_hash+ is either <tt>parent.modules_hash</tt> or
  # <tt>parent.classes_hash</tt> and +all_hash+ is ::all_modules_hash or
  # ::all_classes_hash.

  def remove_nodoc_children
    prefix = self.full_name + '::'

    modules_hash.each_key do |name|
      full_name = prefix + name
      modules_hash.delete name unless RDoc::TopLevel.all_modules_hash[full_name]
    end

    classes_hash.each_key do |name|
      full_name = prefix + name
      classes_hash.delete name unless RDoc::TopLevel.all_classes_hash[full_name]
    end
  end

  ##
  # Get the superclass of this class.  Attempts to retrieve the superclass
  # object, returns the name if it is not known.

  def superclass
    RDoc::TopLevel.find_class_named(@superclass) || @superclass
  end

  ##
  # Set the superclass of this class to +superclass+

  def superclass=(superclass)
    raise NoMethodError, "#{full_name} is a module" if module?
    @superclass = superclass
  end

  def to_s # :nodoc:
    if is_alias_for then
      "#{self.class.name} #{self.full_name} -> #{is_alias_for}"
    else
      super
    end
  end

  ##
  # 'module' or 'class'

  def type
    module? ? 'module' : 'class'
  end

  ##
  # Updates the child modules & classes by replacing the ones that are
  # aliases through a constant.
  #
  # The aliased module/class is replaced in the children and in
  # RDoc::TopLevel::all_modules_hash or RDoc::TopLevel::all_classes_hash
  # by a copy that has <tt>RDoc::ClassModule#is_alias_for</tt> set to
  # the aliased module/class, and this copy is added to <tt>#aliases</tt>
  # of the aliased module/class.
  #
  # Formatters can use the #non_aliases method to retrieve children that
  # are not aliases, for instance to list the namespace content, since
  # the aliased modules are included in the constants of the class/module,
  # that are listed separately.

  def update_aliases
    constants.each do |const|
      next unless cm = const.is_alias_for
      cm_alias = cm.dup
      cm_alias.name = const.name
      cm_alias.parent = self
      cm_alias.full_name = nil  # force update for new parent
      cm_alias.aliases.clear
      cm_alias.is_alias_for = cm

      if cm.module? then
        RDoc::TopLevel.all_modules_hash[cm_alias.full_name] = cm_alias
        modules_hash[const.name] = cm_alias
      else
        RDoc::TopLevel.all_classes_hash[cm_alias.full_name] = cm_alias
        classes_hash[const.name] = cm_alias
      end

      cm.aliases << cm_alias
    end
  end

  ##
  # Deletes from #includes those whose module has been removed from the
  # documentation.
  #--
  # FIXME: includes are not reliably removed, see _possible_bug test case

  def update_includes
    includes.reject! do |include|
      mod = include.module
      !(String === mod) && RDoc::TopLevel.all_modules_hash[mod.full_name].nil?
    end
  end

end

