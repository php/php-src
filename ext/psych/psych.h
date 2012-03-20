#ifndef PSYCH_H
#define PSYCH_H

#include <ruby.h>

#ifdef HAVE_RUBY_ENCODING_H
#include <ruby/encoding.h>
#endif

#include <yaml.h>

#include <parser.h>
#include <emitter.h>
#include <to_ruby.h>
#include <yaml_tree.h>

extern VALUE mPsych;


#endif
