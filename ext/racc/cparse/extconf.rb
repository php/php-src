# $Id$

require 'mkmf'
have_func('rb_block_call', 'ruby/ruby.h')
create_makefile 'racc/cparse'
