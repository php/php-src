require 'date'

module Psych
  DEPRECATED = __FILE__ # :nodoc:

  module DeprecatedMethods # :nodoc:
    attr_accessor :taguri
    attr_accessor :to_yaml_style
  end

  def self.quick_emit thing, opts = {}, &block # :nodoc:
    warn "#{caller[0]}: YAML.quick_emit is deprecated" if $VERBOSE && !caller[0].start_with?(File.dirname(__FILE__))
    target = eval 'self', block.binding
    target.extend DeprecatedMethods
    metaclass = class << target; self; end
    metaclass.send(:define_method, :encode_with) do |coder|
      target.taguri        = coder.tag
      target.to_yaml_style = coder.style
      block.call coder
    end
    target.psych_to_yaml unless opts[:nodump]
  end

  def self.load_documents yaml, &block
    if $VERBOSE
      warn "#{caller[0]}: load_documents is deprecated, use load_stream"
    end
    list = load_stream yaml
    return list unless block_given?
    list.each(&block)
  end

  def self.detect_implicit thing
    warn "#{caller[0]}: detect_implicit is deprecated" if $VERBOSE
    return '' unless String === thing
    return 'null' if '' == thing
    ScalarScanner.new.tokenize(thing).class.name.downcase
  end

  def self.add_ruby_type type_tag, &block
    warn "#{caller[0]}: add_ruby_type is deprecated, use add_domain_type" if $VERBOSE
    domain = 'ruby.yaml.org,2002'
    key = ['tag', domain, type_tag].join ':'
    @domain_types[key] = [key, block]
  end

  def self.add_private_type type_tag, &block
    warn "#{caller[0]}: add_private_type is deprecated, use add_domain_type" if $VERBOSE
    domain = 'x-private'
    key = [domain, type_tag].join ':'
    @domain_types[key] = [key, block]
  end

  def self.tagurize thing
    warn "#{caller[0]}: add_private_type is deprecated, use add_domain_type" if $VERBOSE
    return thing unless String === thing
    "tag:yaml.org,2002:#{thing}"
  end

  def self.read_type_class type, reference
    warn "#{caller[0]}: read_type_class is deprecated" if $VERBOSE
    _, _, type, name = type.split ':', 4

    reference = name.split('::').inject(reference) do |k,n|
      k.const_get(n.to_sym)
    end if name
    [type, reference]
  end

  def self.object_maker klass, hash
    warn "#{caller[0]}: object_maker is deprecated" if $VERBOSE
    klass.allocate.tap do |obj|
      hash.each { |k,v| obj.instance_variable_set(:"@#{k}", v) }
    end
  end
end

class Object
  undef :to_yaml_properties rescue nil
  def to_yaml_properties # :nodoc:
    instance_variables
  end
end
