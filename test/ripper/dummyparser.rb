#
# dummyparser.rb
#

require 'ripper'

class Node
  def initialize(name, *nodes)
    @name = name
    @children = nodes
  end

  attr_reader :name, :children

  def to_s
    "#{@name}(#{Node.trim_nil(@children).map {|n| n.to_s }.join(',')})"
  end

  def self.trim_nil(list)
    if !list.empty? and list.last.nil?
      list = list[0...-1]
      list.pop while !list.empty? and list.last.nil?
    end
    list
  end
end

class NodeList
  def initialize
    @list = []
  end

  attr_reader :list

  def push(item)
    @list.push item
    self
  end

  def prepend(items)
    @list.unshift items
  end

  def to_s
    "[#{@list.join(',')}]"
  end
end

class DummyParser < Ripper
  def hook(*names)
    class << self; self; end.class_eval do
      names.each do |name|
        define_method(name) do |*a, &b|
          result = super(*a, &b)
          yield(name, *a)
          result
        end
      end
    end
    self
  end

  def on_program(stmts)
    stmts
  end

  def on_stmts_new
    NodeList.new
  end

  def on_stmts_add(stmts, st)
    stmts.push st
    stmts
  end

  def on_void_stmt
    Node.new('void')
  end

  def on_var_ref(name)
    Node.new('ref', name)
  end

  def on_var_alias(a, b)
    Node.new('valias', a, b)
  end

  def on_alias_error(a)
    Node.new('aliaserr', a)
  end

  def on_arg_paren(args)
    args
  end

  def on_args_new
    NodeList.new
  end

  def on_args_add(list, arg)
    list.push(arg)
  end

  def on_args_add_block(list, blk)
    if blk
      list.push('&' + blk.to_s)
    else
      list
    end
  end

  def on_args_add_star(list, arg)
    list.push('*' + arg.to_s)
  end

  def on_args_prepend(list, args)
    list.prepend args
    list
  end

  def on_method_add_arg(m, arg)
    if arg == nil
      arg = on_args_new
    end
    m.children.push arg
    m
  end

  def on_method_add_block(m, b)
    on_args_add_block(m.children, b)
    m
  end

  def on_paren(params)
    params
  end

  def on_brace_block(params, code)
    Node.new('block', params, code)
  end

  def on_block_var(params, shadow)
    params
  end

  def on_rest_param(var)
    "*#{var}"
  end

  def on_blockarg(var)
    "&#{var}"
  end

  def on_params(required, optional, rest, more, keyword, keyword_rest, block)
    args = NodeList.new

    required.each do |req|
      args.push(req)
    end if required

    optional.each do |var, val|
      args.push("#{var}=#{val}")
    end if optional

    args.push(rest) if rest

    more.each do |m|
      args.push(m)
    end if more

    args.push(block) if block
    args
  end

  def on_assoc_new(a, b)
    Node.new('assoc', a, b)
  end

  def on_bare_assoc_hash(assoc_list)
    Node.new('assocs', *assoc_list)
  end

  def on_assoclist_from_args(a)
    Node.new('assocs', *a)
  end

  def on_word_new
    ""
  end

  def on_word_add(word, w)
    word << w
  end

  def on_words_new
    NodeList.new
  end

  def on_words_add(words, word)
    words.push word
  end

  def on_qwords_new
    NodeList.new
  end

  def on_qwords_add(words, word)
    words.push word
  end

  (Ripper::PARSER_EVENTS.map(&:to_s) - instance_methods(false).map {|n|n.to_s.sub(/^on_/, '')}).each do |event|
    define_method(:"on_#{event}") do |*args|
      Node.new(event, *args)
    end
  end
end
