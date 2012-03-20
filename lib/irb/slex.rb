#
#   irb/slex.rb - simple lex analyzer
#   	$Release Version: 0.9.6$
#   	$Revision$
#   	by Keiju ISHITSUKA(keiju@ruby-lang.org)
#
# --
#
#
#

require "e2mmap"
require "irb/notifier"

module IRB
  class SLex
    @RCS_ID='-$Id$-'

    extend Exception2MessageMapper
    def_exception :ErrNodeNothing, "node nothing"
    def_exception :ErrNodeAlreadyExists, "node already exists"

    DOUT = Notifier::def_notifier("SLex::")
    D_WARN = DOUT::def_notifier(1, "Warn: ")
    D_DEBUG = DOUT::def_notifier(2, "Debug: ")
    D_DETAIL = DOUT::def_notifier(4, "Detail: ")

    DOUT.level = Notifier::D_NOMSG

    def initialize
      @head = Node.new("")
    end

    def def_rule(token, preproc = nil, postproc = nil, &block)
      D_DETAIL.pp token

      postproc = block if block_given?
      create(token, preproc, postproc)
    end

    def def_rules(*tokens, &block)
      if block_given?
	p = block
      end
      for token in tokens
	def_rule(token, nil, p)
      end
    end

    def preproc(token, proc)
      node = search(token)
      node.preproc=proc
    end

    #要チェック?
    def postproc(token)
      node = search(token, proc)
      node.postproc=proc
    end

    def search(token)
      @head.search(token.split(//))
    end

    def create(token, preproc = nil, postproc = nil)
      @head.create_subnode(token.split(//), preproc, postproc)
    end

    def match(token)
      case token
      when Array
      when String
	return match(token.split(//))
      else
	return @head.match_io(token)
      end
      ret = @head.match(token)
      D_DETAIL.exec_if{D_DEATIL.printf "match end: %s:%s\n", ret, token.inspect}
      ret
    end

    def inspect
      format("<SLex: @head = %s>", @head.inspect)
    end

    #----------------------------------------------------------------------
    #
    #   class Node -
    #
    #----------------------------------------------------------------------
    class Node
      # if postproc is nil, this node is an abstract node.
      # if postproc is non-nil, this node is a real node.
      def initialize(preproc = nil, postproc = nil)
	@Tree = {}
	@preproc = preproc
	@postproc = postproc
      end

      attr_accessor :preproc
      attr_accessor :postproc

      def search(chrs, opt = nil)
	return self if chrs.empty?
	ch = chrs.shift
	if node = @Tree[ch]
	  node.search(chrs, opt)
	else
	  if opt
	    chrs.unshift ch
	    self.create_subnode(chrs)
	  else
	    SLex.fail ErrNodeNothing
	  end
	end
      end

      def create_subnode(chrs, preproc = nil, postproc = nil)
	if chrs.empty?
	  if @postproc
	    D_DETAIL.pp node
	    SLex.fail ErrNodeAlreadyExists
	  else
	    D_DEBUG.puts "change abstract node to real node."
	    @preproc = preproc
	    @postproc = postproc
	  end
	  return self
	end

	ch = chrs.shift
	if node = @Tree[ch]
	  if chrs.empty?
	    if node.postproc
	      DebugLogger.pp node
	      DebugLogger.pp self
	      DebugLogger.pp ch
	      DebugLogger.pp chrs
	      SLex.fail ErrNodeAlreadyExists
	    else
	      D_WARN.puts "change abstract node to real node"
	      node.preproc = preproc
	      node.postproc = postproc
	    end
	  else
	    node.create_subnode(chrs, preproc, postproc)
	  end
	else
	  if chrs.empty?
	    node = Node.new(preproc, postproc)
	  else
	    node = Node.new
	    node.create_subnode(chrs, preproc, postproc)
	  end
	  @Tree[ch] = node
	end
	node
      end

      #
      # chrs: String
      #       character array
      #       io must have getc()/ungetc(); and ungetc() must be
      #       able to be called arbitrary number of times.
      #
      def match(chrs, op = "")
	D_DETAIL.print "match>: ", chrs, "op:", op, "\n"
	if chrs.empty?
	  if @preproc.nil? || @preproc.call(op, chrs)
	    DOUT.printf(D_DETAIL, "op1: %s\n", op)
	    @postproc.call(op, chrs)
	  else
	    nil
	  end
	else
	  ch = chrs.shift
	  if node = @Tree[ch]
	    if ret = node.match(chrs, op+ch)
	      return ret
	    else
	      chrs.unshift ch
	      if @postproc and @preproc.nil? || @preproc.call(op, chrs)
		DOUT.printf(D_DETAIL, "op2: %s\n", op.inspect)
		ret = @postproc.call(op, chrs)
		return ret
	      else
		return nil
	      end
	    end
	  else
	    chrs.unshift ch
	    if @postproc and @preproc.nil? || @preproc.call(op, chrs)
	      DOUT.printf(D_DETAIL, "op3: %s\n", op)
	      @postproc.call(op, chrs)
	      return ""
	    else
	      return nil
	    end
	  end
	end
      end

      def match_io(io, op = "")
	if op == ""
	  ch = io.getc
	  if ch == nil
	    return nil
	  end
	else
	  ch = io.getc_of_rests
	end
	if ch.nil?
	  if @preproc.nil? || @preproc.call(op, io)
	    D_DETAIL.printf("op1: %s\n", op)
	    @postproc.call(op, io)
	  else
	    nil
	  end
	else
	  if node = @Tree[ch]
	    if ret = node.match_io(io, op+ch)
	      ret
	    else
	      io.ungetc ch
	      if @postproc and @preproc.nil? || @preproc.call(op, io)
		DOUT.exec_if{D_DETAIL.printf "op2: %s\n", op.inspect}
		@postproc.call(op, io)
	      else
		nil
	      end
	    end
	  else
	    io.ungetc ch
	    if @postproc and @preproc.nil? || @preproc.call(op, io)
	      D_DETAIL.printf("op3: %s\n", op)
	      @postproc.call(op, io)
	    else
	      nil
	    end
	  end
	end
      end
    end
  end
end

if $0 == __FILE__
  #    Tracer.on
  case $1
  when "1"
    tr = SLex.new
    print "0: ", tr.inspect, "\n"
    tr.def_rule("=") {print "=\n"}
    print "1: ", tr.inspect, "\n"
    tr.def_rule("==") {print "==\n"}
    print "2: ", tr.inspect, "\n"

    print "case 1:\n"
    print tr.match("="), "\n"
    print "case 2:\n"
    print tr.match("=="), "\n"
    print "case 3:\n"
    print tr.match("=>"), "\n"

  when "2"
    tr = SLex.new
    print "0: ", tr.inspect, "\n"
    tr.def_rule("=") {print "=\n"}
    print "1: ", tr.inspect, "\n"
    tr.def_rule("==", proc{false}) {print "==\n"}
    print "2: ", tr.inspect, "\n"

    print "case 1:\n"
    print tr.match("="), "\n"
    print "case 2:\n"
    print tr.match("=="), "\n"
    print "case 3:\n"
    print tr.match("=>"), "\n"
  end
  exit
end

