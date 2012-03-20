require 'rdoc/code_objects'
require 'fileutils'

##
# A set of ri data.
#
# The store manages reading and writing ri data for a project (gem, path,
# etc.) and maintains a cache of methods, classes and ancestors in the
# store.
#
# The store maintains a #cache of its contents for faster lookup.  After
# adding items to the store it must be flushed using #save_cache.  The cache
# contains the following structures:
#
#    @cache = {
#      :class_methods    => {}, # class name => class methods
#      :instance_methods => {}, # class name => instance methods
#      :attributes       => {}, # class name => attributes
#      :modules          => [], # classes and modules in this store
#      :ancestors        => {}, # class name => ancestor names
#    }
#--
# TODO need to store the list of files and prune classes

class RDoc::RI::Store

  ##
  # If true this Store will not write any files

  attr_accessor :dry_run

  ##
  # Path this store reads or writes

  attr_accessor :path

  ##
  # Type of ri datastore this was loaded from.  See RDoc::RI::Driver,
  # RDoc::RI::Paths.

  attr_accessor :type

  ##
  # The contents of the Store

  attr_reader :cache

  ##
  # The encoding of the contents in the Store

  attr_accessor :encoding

  ##
  # Creates a new Store of +type+ that will load or save to +path+

  def initialize path, type = nil
    @dry_run  = false
    @type     = type
    @path     = path
    @encoding = nil

    @cache = {
      :ancestors        => {},
      :attributes       => {},
      :class_methods    => {},
      :encoding         => @encoding,
      :instance_methods => {},
      :modules          => [],
    }
  end

  ##
  # Ancestors cache accessor.  Maps a klass name to an Array of its ancestors
  # in this store.  If Foo in this store inherits from Object, Kernel won't be
  # listed (it will be included from ruby's ri store).

  def ancestors
    @cache[:ancestors]
  end

  ##
  # Attributes cache accessor.  Maps a class to an Array of its attributes.

  def attributes
    @cache[:attributes]
  end

  ##
  # Path to the cache file

  def cache_path
    File.join @path, 'cache.ri'
  end

  ##
  # Path to the ri data for +klass_name+

  def class_file klass_name
    name = klass_name.split('::').last
    File.join class_path(klass_name), "cdesc-#{name}.ri"
  end

  ##
  # Class methods cache accessor.  Maps a class to an Array of its class
  # methods (not full name).

  def class_methods
    @cache[:class_methods]
  end

  ##
  # Path where data for +klass_name+ will be stored (methods or class data)

  def class_path klass_name
    File.join @path, *klass_name.split('::')
  end

  ##
  # Removes empty items and ensures item in each collection are unique and
  # sorted

  def clean_cache_collection collection # :nodoc:
    collection.each do |name, item|
      if item.empty? then
        collection.delete name
      else
        # HACK mongrel-1.1.5 documents its files twice
        item.uniq!
        item.sort!
      end
    end
  end

  ##
  # Friendly rendition of #path

  def friendly_path
    case type
    when :gem    then
      sep = Regexp.union(*['/', File::ALT_SEPARATOR].compact)
      @path =~ /#{sep}doc#{sep}(.*?)#{sep}ri$/
      "gem #{$1}"
    when :home   then '~/.ri'
    when :site   then 'ruby site'
    when :system then 'ruby core'
    else @path
    end
  end

  def inspect # :nodoc:
    "#<%s:0x%x %s %p>" % [self.class, object_id, @path, modules.sort]
  end

  ##
  # Instance methods cache accessor.  Maps a class to an Array of its
  # instance methods (not full name).

  def instance_methods
    @cache[:instance_methods]
  end

  ##
  # Loads cache file for this store

  def load_cache
    #orig_enc = @encoding

    open cache_path, 'rb' do |io|
      @cache = Marshal.load io.read
    end

    load_enc = @cache[:encoding]

    # TODO this feature will be time-consuming to add:
    # a) Encodings may be incompatible but transcodeable
    # b) Need to warn in the appropriate spots, wherever they may be
    # c) Need to handle cross-cache differences in encodings
    # d) Need to warn when generating into a cache with diffent encodings
    #
    #if orig_enc and load_enc != orig_enc then
    #  warn "Cached encoding #{load_enc} is incompatible with #{orig_enc}\n" \
    #       "from #{path}/cache.ri" unless
    #    Encoding.compatible? orig_enc, load_enc
    #end

    @encoding = load_enc unless @encoding

    @cache
  rescue Errno::ENOENT
  end

  ##
  # Loads ri data for +klass_name+

  def load_class klass_name
    open class_file(klass_name), 'rb' do |io|
      Marshal.load io.read
    end
  end

  ##
  # Loads ri data for +method_name+ in +klass_name+

  def load_method klass_name, method_name
    open method_file(klass_name, method_name), 'rb' do |io|
      Marshal.load io.read
    end
  end

  ##
  # Path to the ri data for +method_name+ in +klass_name+

  def method_file klass_name, method_name
    method_name = method_name.split('::').last
    method_name =~ /#(.*)/
    method_type = $1 ? 'i' : 'c'
    method_name = $1 if $1

    method_name = if ''.respond_to? :ord then
                    method_name.gsub(/\W/) { "%%%02x" % $&[0].ord }
                  else
                    method_name.gsub(/\W/) { "%%%02x" % $&[0] }
                  end

    File.join class_path(klass_name), "#{method_name}-#{method_type}.ri"
  end

  ##
  # Modules cache accessor.  An Array of all the modules (and classes) in the
  # store.

  def modules
    @cache[:modules]
  end

  ##
  # Writes the cache file for this store

  def save_cache
    clean_cache_collection @cache[:ancestors]
    clean_cache_collection @cache[:attributes]
    clean_cache_collection @cache[:class_methods]
    clean_cache_collection @cache[:instance_methods]

    @cache[:modules].uniq!
    @cache[:modules].sort!
    @cache[:encoding] = @encoding # this gets set twice due to assert_cache

    return if @dry_run

    marshal = Marshal.dump @cache

    open cache_path, 'wb' do |io|
      io.write marshal
    end
  end

  ##
  # Writes the ri data for +klass+

  def save_class klass
    full_name = klass.full_name

    FileUtils.mkdir_p class_path(full_name) unless @dry_run

    @cache[:modules] << full_name

    path = class_file full_name

    begin
      disk_klass = load_class full_name

      klass = disk_klass.merge klass
    rescue Errno::ENOENT
    end

    # BasicObject has no ancestors
    ancestors = klass.ancestors.compact.map do |ancestor|
      # HACK for classes we don't know about (class X < RuntimeError)
      String === ancestor ? ancestor : ancestor.full_name
    end

    @cache[:ancestors][full_name] ||= []
    @cache[:ancestors][full_name].push(*ancestors)

    attributes = klass.attributes.map do |attribute|
      "#{attribute.definition} #{attribute.name}"
    end

    unless attributes.empty? then
      @cache[:attributes][full_name] ||= []
      @cache[:attributes][full_name].push(*attributes)
    end

    to_delete = []

    unless klass.method_list.empty? then
      @cache[:class_methods][full_name]    ||= []
      @cache[:instance_methods][full_name] ||= []

      class_methods, instance_methods =
        klass.method_list.partition { |meth| meth.singleton }

      class_methods    = class_methods.   map { |method| method.name }
      instance_methods = instance_methods.map { |method| method.name }

      old = @cache[:class_methods][full_name] - class_methods
      to_delete.concat old.map { |method|
        method_file full_name, "#{full_name}::#{method}"
      }

      old = @cache[:instance_methods][full_name] - instance_methods
      to_delete.concat old.map { |method|
        method_file full_name, "#{full_name}##{method}"
      }

      @cache[:class_methods][full_name]    = class_methods
      @cache[:instance_methods][full_name] = instance_methods
    end

    return if @dry_run

    FileUtils.rm_f to_delete

    marshal = Marshal.dump klass

    open path, 'wb' do |io|
      io.write marshal
    end
  end

  ##
  # Writes the ri data for +method+ on +klass+

  def save_method klass, method
    full_name = klass.full_name

    FileUtils.mkdir_p class_path(full_name) unless @dry_run

    cache = if method.singleton then
              @cache[:class_methods]
            else
              @cache[:instance_methods]
            end
    cache[full_name] ||= []
    cache[full_name] << method.name

    return if @dry_run

    marshal = Marshal.dump method

    open method_file(full_name, method.full_name), 'wb' do |io|
      io.write marshal
    end
  end

end

