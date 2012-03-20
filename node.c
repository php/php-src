/**********************************************************************

  node.c - ruby node tree

  $Author: mame $
  created at: 09/12/06 21:23:44 JST

  Copyright (C) 2009 Yusuke Endoh

**********************************************************************/

#include "ruby/ruby.h"
#include "vm_core.h"

#define A(str) rb_str_cat2(buf, (str))
#define AR(str) rb_str_concat(buf, (str))

#define A_INDENT add_indent(buf, indent)
#define A_ID(id) add_id(buf, (id))
#define A_INT(val) rb_str_catf(buf, "%d", (val));
#define A_LONG(val) rb_str_catf(buf, "%ld", (val));
#define A_LIT(lit) AR(rb_inspect(lit))
#define A_NODE_HEADER(node) \
    rb_str_catf(buf, "@ %s (line: %d)", ruby_node_name(nd_type(node)), nd_line(node))
#define A_FIELD_HEADER(name) \
    rb_str_catf(buf, "+- %s:", (name))

#define D_NULL_NODE A_INDENT; A("(null node)"); A("\n");
#define D_NODE_HEADER(node) A_INDENT; A_NODE_HEADER(node); A("\n");

#define COMPOUND_FIELD(name, name2, block) \
    do { \
	A_INDENT; A_FIELD_HEADER(comment ? (name2) : (name)); A("\n"); \
	rb_str_cat2(indent, next_indent); \
	block; \
	rb_str_resize(indent, RSTRING_LEN(indent) - 4); \
    } while (0)

#define SIMPLE_FIELD(name, name2, block) \
    do { \
	A_INDENT; A_FIELD_HEADER(comment ? (name2) : (name)); A(" "); block; A("\n"); \
    } while (0)

#define F_CUSTOM1(name, ann, block) SIMPLE_FIELD(#name, #name " (" ann ")", block)
#define F_ID(name, ann)		    SIMPLE_FIELD(#name, #name " (" ann ")", A_ID(node->name))
#define F_GENTRY(name, ann)	    SIMPLE_FIELD(#name, #name " (" ann ")", A_ID((node->name)->id))
#define F_INT(name, ann)	    SIMPLE_FIELD(#name, #name " (" ann ")", A_INT(node->name))
#define F_LONG(name, ann)	    SIMPLE_FIELD(#name, #name " (" ann ")", A_LONG(node->name))
#define F_LIT(name, ann)	    SIMPLE_FIELD(#name, #name " (" ann ")", A_LIT(node->name))
#define F_MSG(name, ann, desc)	    SIMPLE_FIELD(#name, #name " (" ann ")", A(desc))

#define F_CUSTOM2(name, ann, block) \
    COMPOUND_FIELD(#name, #name " (" ann ")", block)

#define F_NODE(name, ann) \
    COMPOUND_FIELD(#name, #name " (" ann ")", dump_node(buf, indent, comment, node->name))

#define ANN(ann) \
    if (comment) { \
	A_INDENT; A("| # "); A(ann); A("\n"); \
    }

#define LAST_NODE (next_indent = "    ")

static void
add_indent(VALUE buf, VALUE indent)
{
    AR(indent);
}

static void
add_id(VALUE buf, ID id)
{
    if (id == 0) {
	A("(null)");
    }
    else {
	VALUE str = rb_id2str(id);
	if (str) {
	    A(":"); AR(str);
	}
	else {
	    A("(internal variable)");
	}
    }
}

static void
dump_node(VALUE buf, VALUE indent, int comment, NODE *node)
{
    const char *next_indent = "|   ";

    if (!node) {
	D_NULL_NODE;
	return;
    }

    D_NODE_HEADER(node);

    switch (nd_type(node)) {
      case NODE_BLOCK:
	ANN("statement sequence");
	ANN("format: [nd_head]; [nd_next]");
	ANN("example: foo; bar");
	F_NODE(nd_head, "current statement");
	LAST_NODE;
	F_NODE(nd_next, "next block");
	break;

      case NODE_IF:
	ANN("if statement");
	ANN("format: if [nd_cond] then [nd_body] else [nd_else] end");
	ANN("example: if x == 1 then foo else bar end");
	F_NODE(nd_cond, "condition expr");
	F_NODE(nd_body, "then clause");
	LAST_NODE;
	F_NODE(nd_else, "else clause");
	break;

      case NODE_CASE:
	ANN("case statement");
	ANN("format: case [nd_head]; [nd_body]; end");
	ANN("example: case x; when 1; foo; when 2; bar; else baz; end");
	F_NODE(nd_head, "case expr");
	LAST_NODE;
	F_NODE(nd_body, "when clauses");
	break;

      case NODE_WHEN:
	ANN("if statement");
	ANN("format: when [nd_head]; [nd_body]; (when or else) [nd_next]");
	ANN("example: case x; when 1; foo; when 2; bar; else baz; end");
	F_NODE(nd_head, "when value");
	F_NODE(nd_body, "when clause");
	LAST_NODE;
	F_NODE(nd_next, "next when clause");
	break;

      case NODE_OPT_N:
	ANN("wrapper for -n option");
	ANN("format: ruby -ne '[nd_body]' (nd_cond is `gets')");
	ANN("example: ruby -ne 'p $_'");
	goto loop;
      case NODE_WHILE:
	ANN("while statement");
	ANN("format: while [nd_cond]; [nd_body]; end");
	ANN("example: while x == 1; foo; end");
	goto loop;
      case NODE_UNTIL:
	ANN("until statement");
	ANN("format: until [nd_cond]; [nd_body]; end");
	ANN("example: until x == 1; foo; end");
      loop:
	F_CUSTOM1(nd_state, "begin-end-while?", {
	   A_INT((int)node->nd_state);
	   A((node->nd_state == 1) ? " (while-end)" : " (begin-end-while)");
	});
	F_NODE(nd_cond, "condition");
	LAST_NODE;
	F_NODE(nd_body, "body");
	break;

      case NODE_ITER:
	ANN("method call with block");
	ANN("format: [nd_iter] { [nd_body] }");
	ANN("example: 3.times { foo }");
	goto iter;
      case NODE_FOR:
	ANN("for statement");
	ANN("format: for * in [nd_iter] do [nd_body] end");
	ANN("example: for i in 1..3 do foo end");
	iter:
	F_NODE(nd_iter, "iteration receiver");
	LAST_NODE;
	F_NODE(nd_body, "body");
	break;

      case NODE_BREAK:
	ANN("for statement");
	ANN("format: break [nd_stts]");
	ANN("example: break 1");
	goto jump;
      case NODE_NEXT:
	ANN("next statement");
	ANN("format: next [nd_stts]");
	ANN("example: next 1");
	goto jump;
      case NODE_RETURN:
	ANN("return statement");
	ANN("format: return [nd_stts]");
	ANN("example: return 1");
        jump:
	LAST_NODE;
	F_NODE(nd_stts, "value");
	break;

      case NODE_REDO:
	ANN("redo statement");
	ANN("format: redo");
	ANN("example: redo");
	break;

      case NODE_RETRY:
	ANN("retry statement");
	ANN("format: retry");
	ANN("example: retry");
	break;

      case NODE_BEGIN:
	ANN("begin statement");
	ANN("format: begin; [nd_body]; end");
	ANN("example: begin; 1; end");
	LAST_NODE;
	F_NODE(nd_body, "body");
	break;

      case NODE_RESCUE:
	ANN("rescue clause");
	ANN("format: begin; [nd_body]; (rescue) [nd_resq]; else [nd_else]; end");
	ANN("example: begin; foo; rescue; bar; else; baz; end");
	F_NODE(nd_head, "body");
	F_NODE(nd_resq, "rescue clause list");
	LAST_NODE;
	F_NODE(nd_else, "rescue else clause");
	break;

      case NODE_RESBODY:
	ANN("rescue clause (cont'd)");
	ANN("format: rescue [nd_args]; [nd_body]; (rescue) [nd_head]");
	ANN("example: begin; foo; rescue; bar; else; baz; end");
	F_NODE(nd_args, "rescue exceptions");
	F_NODE(nd_body, "rescue clause");
	LAST_NODE;
	F_NODE(nd_head, "next rescue clause");
	break;

      case NODE_ENSURE:
	ANN("ensure clause");
	ANN("format: begin; [nd_head]; ensure; [nd_ensr]; end");
	ANN("example: begin; foo; ensure; bar; end");
	F_NODE(nd_head, "body");
	LAST_NODE;
	F_NODE(nd_ensr, "ensure clause");
	break;

      case NODE_AND:
	ANN("&& operator");
	ANN("format: [nd_1st] && [nd_2nd]");
	ANN("example: foo && bar");
	goto andor;
      case NODE_OR:
	ANN("|| operator");
	ANN("format: [nd_1st] || [nd_2nd]");
	ANN("example: foo && bar");
	andor:
	F_NODE(nd_1st, "left expr");
	LAST_NODE;
	F_NODE(nd_2nd, "right expr");
	break;

      case NODE_MASGN:
	ANN("multiple assignment");
	ANN("format: [nd_head], [nd_args] = [nd_value]");
	ANN("example: a, b = foo");
	F_NODE(nd_value, "rhsn");
	F_NODE(nd_head, "lhsn");
	if ((VALUE)node->nd_args != (VALUE)-1) {
	    LAST_NODE;
	    F_NODE(nd_args, "splatn");
	}
	else {
	    F_MSG(nd_args, "splatn", "-1 (rest argument without name)");
	}
	break;

      case NODE_LASGN:
	ANN("local variable assignment");
	ANN("format: [nd_vid](lvar) = [nd_value]");
	ANN("example: x = foo");
	goto asgn;
      case NODE_DASGN:
	ANN("dynamic variable assignment (out of current scope)");
	ANN("format: [nd_vid](dvar) = [nd_value]");
	ANN("example: x = nil; 1.times { x = foo }");
	goto asgn;
      case NODE_DASGN_CURR:
	ANN("dynamic variable assignment (in current scope)");
	ANN("format: [nd_vid](current dvar) = [nd_value]");
	ANN("example: 1.times { x = foo }");
	goto asgn;
      case NODE_IASGN:
	ANN("instance variable assignment");
	ANN("format: [nd_vid](ivar) = [nd_value]");
	ANN("example: @x = foo");
	goto asgn;
      case NODE_CVASGN:
	ANN("class variable assignment");
	ANN("format: [nd_vid](cvar) = [nd_value]");
	ANN("example: @@x = foo");
	asgn:
	F_ID(nd_vid, "variable");
	LAST_NODE;
	F_NODE(nd_value, "rvalue");
	break;

      case NODE_GASGN:
	ANN("global variable assignment");
	ANN("format: [nd_entry](gvar) = [nd_value]");
	ANN("example: $x = foo");
	F_GENTRY(nd_entry, "global variable");
	LAST_NODE;
	F_NODE(nd_value, "rvalue");
	break;

      case NODE_CDECL:
	ANN("constant declaration");
	ANN("format: [nd_else]::[nd_vid](constant) = [nd_value]");
	ANN("example: X = foo");
	if (node->nd_vid) {
	   F_ID(nd_vid, "variable");
	   F_MSG(nd_else, "extension", "not used");
	}
	else {
	   F_MSG(nd_vid, "variable", "0 (see extension field)");
	   F_NODE(nd_else, "extension");
	}
	LAST_NODE;
	F_NODE(nd_value, "rvalue");
	break;

      case NODE_OP_ASGN1:
	ANN("array assignment with operator");
	ANN("format: [nd_value] [ [nd_args->nd_body] ] [nd_vid]= [nd_args->nd_head]");
	ANN("example: ary[1] += foo");
	F_NODE(nd_recv, "receiver");
	F_ID(nd_vid, "operator");
	F_NODE(nd_args->nd_body, "index");
	LAST_NODE;
	F_NODE(nd_args->nd_head, "rvalue");
	break;

      case NODE_OP_ASGN2:
	ANN("attr assignment with operator");
	ANN("format: [nd_value].[attr] [nd_next->nd_mid]= [nd_value]");
	ANN("          where [attr] reader: [nd_next->nd_vid]");
	ANN("                [attr] writer: [nd_next->nd_aid]");
	ANN("example: struct.field += foo");
	F_NODE(nd_recv, "receiver");
	F_ID(nd_next->nd_vid, "reader");
	F_ID(nd_next->nd_aid, "writer");
	F_CUSTOM1(nd_next->nd_mid, "operator", {
	   switch (node->nd_next->nd_mid) {
	   case 0: A("0 (||)"); break;
	   case 1: A("1 (&&)"); break;
	   default: A_ID(node->nd_next->nd_mid);
	   }
	});
	LAST_NODE;
	F_NODE(nd_value, "rvalue");
	break;

      case NODE_OP_ASGN_AND:
	ANN("assignment with && operator");
	ANN("format: [nd_head] &&= [nd_value]");
	ANN("example: foo &&= bar");
	goto asgn_andor;
      case NODE_OP_ASGN_OR:
	ANN("assignment with || operator");
	ANN("format: [nd_head] ||= [nd_value]");
	ANN("example: foo ||= bar");
	asgn_andor:
	F_NODE(nd_head, "variable");
	LAST_NODE;
	F_NODE(nd_value, "rvalue");
	break;

      case NODE_CALL:
	ANN("method invocation");
	ANN("format: [nd_recv].[nd_mid]([nd_args])");
	ANN("example: obj.foo(1)");
	F_ID(nd_mid, "method id");
	F_NODE(nd_recv, "receiver");
	LAST_NODE;
	F_NODE(nd_args, "arguments");
	break;

      case NODE_FCALL:
	ANN("function call");
	ANN("format: [nd_mid]([nd_args])");
	ANN("example: foo(1)");
	F_ID(nd_mid, "method id");
	LAST_NODE;
	F_NODE(nd_args, "arguments");
	break;

      case NODE_VCALL:
	ANN("function call with no argument");
	ANN("format: [nd_mid]");
	ANN("example: foo");
	F_ID(nd_mid, "method id");
	break;

      case NODE_SUPER:
	ANN("super invocation");
	ANN("format: super [nd_args]");
	ANN("example: super 1");
	LAST_NODE;
	F_NODE(nd_args, "arguments");
	break;

      case NODE_ZSUPER:
	ANN("super invocation with no argument");
	ANN("format: super");
	ANN("example: super");
	break;

      case NODE_ARRAY:
	ANN("array constructor");
	ANN("format: [ [nd_head], [nd_next].. ] (length: [nd_alen])");
	ANN("example: [1, 2, 3]");
	goto ary;
      case NODE_VALUES:
	ANN("return arguments");
	ANN("format: [ [nd_head], [nd_next].. ] (length: [nd_alen])");
	ANN("example: return 1, 2, 3");
	ary:
	F_LONG(nd_alen, "length");
	F_NODE(nd_head, "element");
	LAST_NODE;
	F_NODE(nd_next, "next element");
	break;

      case NODE_ZARRAY:
	ANN("empty array constructor");
	ANN("format: []");
	ANN("example: []");
	break;

      case NODE_HASH:
	ANN("hash constructor");
	ANN("format: { [nd_head] }");
	ANN("example: { 1 => 2, 3 => 4 }");
	LAST_NODE;
	F_NODE(nd_head, "contents");
	break;

      case NODE_YIELD:
	ANN("yield invocation");
	ANN("format: yield [nd_head]");
	ANN("example: yield 1");
	LAST_NODE;
	F_NODE(nd_head, "arguments");
	break;

      case NODE_LVAR:
	ANN("local variable reference");
	ANN("format: [nd_vid](lvar)");
	ANN("example: x");
	goto var;
      case NODE_DVAR:
	ANN("dynamic variable reference");
	ANN("format: [nd_vid](dvar)");
	ANN("example: 1.times { x = 1; x }");
	goto var;
      case NODE_IVAR:
	ANN("instance variable reference");
	ANN("format: [nd_vid](ivar)");
	ANN("example: @x");
	goto var;
      case NODE_CONST:
	ANN("constant reference");
	ANN("format: [nd_vid](constant)");
	ANN("example: X");
	goto var;
      case NODE_CVAR:
	ANN("class variable reference");
	ANN("format: [nd_vid](cvar)");
	ANN("example: @@x");
        var:
	F_ID(nd_vid, "local variable");
	break;

      case NODE_GVAR:
	ANN("global variable reference");
	ANN("format: [nd_entry](gvar)");
	ANN("example: $x");
	F_GENTRY(nd_entry, "global variable");
	break;

      case NODE_NTH_REF:
	ANN("nth special variable reference");
	ANN("format: $[nd_nth]");
	ANN("example: $1, $2, ..");
	F_CUSTOM1(nd_nth, "variable", { A("$"); A_LONG(node->nd_nth); });
	break;

      case NODE_BACK_REF:
	ANN("back special variable reference");
	ANN("format: $[nd_nth]");
	ANN("example: $&, $`, $', $+");
	F_CUSTOM1(nd_nth, "variable", {
	   char name[3];
	   name[0] = '$';
	   name[1] = (char)node->nd_nth;
	   name[2] = '\0';
	   A(name);
	});
	break;

      case NODE_MATCH:
	ANN("match expression (against $_ implicitly)");
        ANN("format: [nd_lit] (in condition)");
	ANN("example: if /foo/; foo; end");
	F_LIT(nd_lit, "regexp");
	break;

      case NODE_MATCH2:
	ANN("match expression (regexp first)");
        ANN("format: [nd_recv] =~ [nd_value]");
	ANN("example: /foo/ =~ 'foo'");
	F_NODE(nd_recv, "regexp (receiver)");
	LAST_NODE;
	F_NODE(nd_value, "string (argument)");
	break;

      case NODE_MATCH3:
	ANN("match expression (regexp second)");
        ANN("format: [nd_recv] =~ [nd_value]");
	ANN("example: 'foo' =~ /foo/");
	F_NODE(nd_recv, "string (receiver)");
	LAST_NODE;
	F_NODE(nd_value, "regexp (argument)");
	break;

      case NODE_LIT:
	ANN("literal");
	ANN("format: [nd_lit]");
	ANN("example: 1, /foo/");
	goto lit;
      case NODE_STR:
	ANN("string literal");
	ANN("format: [nd_lit]");
	ANN("example: 'foo'");
	goto lit;
      case NODE_XSTR:
	ANN("xstring literal");
	ANN("format: [nd_lit]");
	ANN("example: `foo`");
	lit:
	F_LIT(nd_lit, "literal");
	break;

      case NODE_DSTR:
	ANN("string literal with interpolation");
	ANN("format: [nd_lit]");
	ANN("example: \"foo#{ bar }baz\"");
	goto dlit;
      case NODE_DXSTR:
	ANN("xstring literal with interpolation");
	ANN("format: [nd_lit]");
	ANN("example: `foo#{ bar }baz`");
	goto dlit;
      case NODE_DREGX:
	ANN("regexp literal with interpolation");
	ANN("format: [nd_lit]");
	ANN("example: /foo#{ bar }baz/");
	goto dlit;
      case NODE_DREGX_ONCE:
	ANN("regexp literal with interpolation and once flag");
	ANN("format: [nd_lit]");
	ANN("example: /foo#{ bar }baz/o");
	goto dlit;
      case NODE_DSYM:
	ANN("symbol literal with interpolation");
	ANN("format: [nd_lit]");
	ANN("example: :\"foo#{ bar }baz\"");
	dlit:
	F_LIT(nd_lit, "literal");
	F_NODE(nd_next->nd_head, "preceding string");
	LAST_NODE;
	F_NODE(nd_next->nd_next, "interpolation");
	break;

      case NODE_EVSTR:
	ANN("interpolation expression");
	ANN("format: \"..#{ [nd_lit] }..\"");
	ANN("example: \"foo#{ bar }baz\"");
	LAST_NODE;
	F_NODE(nd_body, "body");
	break;

      case NODE_ARGSCAT:
	ANN("splat argument following arguments");
	ANN("format: ..(*[nd_head], [nd_body..])");
	ANN("example: foo(*ary, post_arg1, post_arg2)");
	F_NODE(nd_head, "preceding array");
	LAST_NODE;
	F_NODE(nd_body, "following array");
	break;

      case NODE_ARGSPUSH:
	ANN("splat argument following one argument");
	ANN("format: ..(*[nd_head], [nd_body])");
	ANN("example: foo(*ary, post_arg)");
	F_NODE(nd_head, "preceding array");
	LAST_NODE;
	F_NODE(nd_body, "following element");
	break;

      case NODE_SPLAT:
	ANN("splat argument");
	ANN("format: *[nd_head]");
	ANN("example: foo(*ary)");
	LAST_NODE;
	F_NODE(nd_head, "splat'ed array");
	break;

      case NODE_BLOCK_PASS:
	ANN("arguments with block argument");
	ANN("format: ..([nd_head], &[nd_body])");
	ANN("example: foo(x, &blk)");
	F_NODE(nd_head, "other arguments");
	LAST_NODE;
	F_NODE(nd_body, "block argument");
	break;

      case NODE_DEFN:
	ANN("method definition");
	ANN("format: def [nd_mid] [nd_defn]; end");
	ANN("example; def foo; bar; end");
	F_ID(nd_mid, "method name");
	LAST_NODE;
	F_NODE(nd_defn, "method definition");
	break;

      case NODE_DEFS:
	ANN("singleton method definition");
	ANN("format: def [nd_recv].[nd_mid] [nd_defn]; end");
	ANN("example; def obj.foo; bar; end");
	F_NODE(nd_recv, "receiver");
	F_ID(nd_mid, "method name");
	LAST_NODE;
	F_NODE(nd_defn, "method definition");
	break;

      case NODE_ALIAS:
	ANN("method alias statement");
	ANN("format: alias [u1.node] [u2.node]");
	ANN("example: alias bar foo");
	F_NODE(u1.node, "new name");
	LAST_NODE;
	F_NODE(u2.node, "old name");
	break;

      case NODE_VALIAS:
	ANN("global variable alias statement");
	ANN("format: alias [u1.id](gvar) [u2.id](gvar)");
	ANN("example: alias $y $x");
	F_ID(u1.id, "new name");
	F_ID(u2.id, "old name");
	break;

      case NODE_UNDEF:
	ANN("method alias statement");
	ANN("format: undef [u2.node]");
	ANN("example: undef foo");
	LAST_NODE;
	F_NODE(u2.node, "old name");
	break;

      case NODE_CLASS:
	ANN("class definition");
	ANN("format: class [nd_cpath] < [nd_super]; [nd_body]; end");
	ANN("example: class C2 < C; ..; end");
	F_NODE(nd_cpath, "class path");
	F_NODE(nd_super, "superclass");
	LAST_NODE;
	F_NODE(nd_body, "class definition");
	break;

      case NODE_MODULE:
	ANN("module definition");
	ANN("format: module [nd_cpath]; [nd_body]; end");
	ANN("example: module M; ..; end");
	F_NODE(nd_cpath, "module path");
	LAST_NODE;
	F_NODE(nd_body, "module definition");
	break;

      case NODE_SCLASS:
	ANN("singleton class definition");
	ANN("format: class << [nd_recv]; [nd_body]; end");
	ANN("example: class << obj; ..; end");
	F_NODE(nd_recv, "receiver");
	LAST_NODE;
	F_NODE(nd_body, "singleton class definition");
	break;

      case NODE_COLON2:
	ANN("scoped constant reference");
	ANN("format: [nd_head]::[nd_mid]");
	ANN("example: M::C");
	F_ID(nd_mid, "constant name");
	LAST_NODE;
	F_NODE(nd_head, "receiver");
	break;

      case NODE_COLON3:
	ANN("top-level constant reference");
	ANN("format: ::[nd_mid]");
	ANN("example: ::Object");
	F_ID(nd_mid, "constant name");
	break;

      case NODE_DOT2:
	ANN("range constructor (incl.)");
	ANN("format: [nd_beg]..[nd_end]");
	ANN("example: 1..5");
	goto dot;
      case NODE_DOT3:
	ANN("range constructor (excl.)");
	ANN("format: [nd_beg]...[nd_end]");
	ANN("example: 1...5");
	goto dot;
      case NODE_FLIP2:
	ANN("flip-flop condition (incl.)");
	ANN("format: [nd_beg]..[nd_end]");
	ANN("example: if (x==1)..(x==5); foo; end");
	goto dot;
      case NODE_FLIP3:
	ANN("flip-flop condition (excl.)");
	ANN("format: [nd_beg]...[nd_end]");
	ANN("example: if (x==1)...(x==5); foo; end");
	dot:
	F_NODE(nd_beg, "begin");
	LAST_NODE;
	F_NODE(nd_end, "end");
	break;

      case NODE_SELF:
	ANN("self");
	ANN("format: self");
	ANN("example: self");
	break;

      case NODE_NIL:
	ANN("nil");
	ANN("format: nil");
	ANN("example: nil");
	break;

      case NODE_TRUE:
	ANN("true");
	ANN("format: true");
	ANN("example: true");
	break;

      case NODE_FALSE:
	ANN("false");
	ANN("format: false");
	ANN("example: false");
	break;

      case NODE_ERRINFO:
	ANN("virtual reference to $!");
	ANN("format: rescue => id");
	ANN("example: rescue => id");
	break;

      case NODE_DEFINED:
	ANN("defined? expression");
	ANN("format: defined?([nd_head])");
	ANN("example: defined?(foo)");
	F_NODE(nd_head, "expr");
	break;

      case NODE_POSTEXE:
	ANN("post-execution");
	ANN("format: END { [nd_body] }");
	ANN("example: END { foo }");
	LAST_NODE;
	F_NODE(nd_body, "END clause");
	break;

      case NODE_ATTRASGN:
	ANN("attr assignment");
	ANN("format: [nd_recv].[nd_mid] = [nd_args]");
	ANN("example: struct.field = foo");
	if (node->nd_recv == (NODE *) 1) {
	    F_MSG(nd_recv, "receiver", "1 (self)");
	}
	else {
	    F_NODE(nd_recv, "receiver");
	}
	F_ID(nd_mid, "method name");
	LAST_NODE;
	F_NODE(nd_args, "arguments");
	break;

      case NODE_PRELUDE:
	ANN("pre-execution");
	ANN("format: BEGIN { [nd_head] }; [nd_body]");
	ANN("example: bar; BEGIN { foo }");
	F_NODE(nd_head, "prelude");
	LAST_NODE;
	F_NODE(nd_body, "body");
	break;

      case NODE_LAMBDA:
	ANN("lambda expression");
	ANN("format: -> [nd_body]");
	ANN("example: -> { foo }");
	LAST_NODE;
	F_NODE(nd_body, "lambda clause");
	break;

      case NODE_OPT_ARG:
	ANN("optional arguments");
	ANN("format: def method_name([nd_body=some], [nd_next..])");
	ANN("example: def foo(a, b=1, c); end");
	F_NODE(nd_body, "body");
	LAST_NODE;
	F_NODE(nd_next, "next");
	break;

      case NODE_KW_ARG:
	ANN("keyword arguments");
	ANN("format: def method_name([nd_body=some], [nd_next..])");
	ANN("example: def foo(a:1, b:2); end");
	F_NODE(nd_body, "body");
	LAST_NODE;
	F_NODE(nd_next, "next");
	break;

      case NODE_POSTARG:
	ANN("post arguments");
	ANN("format: *[nd_1st], [nd_2nd..] = ..");
	ANN("example: a, *rest, z = foo");
	if ((VALUE)node->nd_1st != (VALUE)-1) {
	    F_NODE(nd_1st, "rest argument");
	}
	else {
	    F_MSG(nd_1st, "rest argument", "-1 (rest argument without name)");
	}
	LAST_NODE;
	F_NODE(nd_2nd, "post arguments");
	break;

      case NODE_ARGS:
	ANN("method parameters");
	ANN("format: def method_name(.., [nd_opt=some], *[nd_rest], [nd_pid], .., &[nd_body])");
	ANN("example: def foo(a, b, opt1=1, opt2=2, *rest, y, z, &blk); end");
	F_INT(nd_ainfo->pre_args_num, "count of mandatory (pre-)arguments");
	F_NODE(nd_ainfo->pre_init, "initialization of (pre-)arguments");
	F_INT(nd_ainfo->post_args_num, "count of mandatory post-arguments");
	F_NODE(nd_ainfo->post_init, "initialization of post-arguments");
	F_ID(nd_ainfo->first_post_arg, "first post argument");
	F_ID(nd_ainfo->rest_arg, "rest argument");
	F_ID(nd_ainfo->block_arg, "block argument");
	F_NODE(nd_ainfo->opt_args, "optional arguments");
	LAST_NODE;
	F_NODE(nd_ainfo->kw_args, "keyword arguments");
	F_NODE(nd_ainfo->kw_rest_arg, "keyword rest argument");
	break;

      case NODE_SCOPE:
	ANN("new scope");
	ANN("format: [nd_tbl]: local table, [nd_args]: arguments, [nd_body]: body");
	F_CUSTOM1(nd_tbl, "local table", {
	   ID *tbl = node->nd_tbl;
	   int i;
	   int size = tbl ? (int)*tbl++ : 0;
	   if (size == 0) A("(empty)");
	   for (i = 0; i < size; i++) {
		A_ID(tbl[i]); if (i < size - 1) A(",");
	   }
	});
	F_NODE(nd_args, "arguments");
	LAST_NODE;
	F_NODE(nd_body, "body");
	break;

      default:
	rb_bug("dump_node: unknown node: %s", ruby_node_name(nd_type(node)));
    }
}

VALUE
rb_parser_dump_tree(NODE *node, int comment)
{
    VALUE buf = rb_str_new_cstr(
	"###########################################################\n"
	"## Do NOT use this node dump for any purpose other than  ##\n"
	"## debug and research.  Compatibility is not guaranteed. ##\n"
	"###########################################################\n\n"
    );
    dump_node(buf, rb_str_new_cstr("# "), comment, node);
    return buf;
}
