/*
 * $Id$
 * 'OpenSSL for Ruby' project
 * Copyright (C) 2001-2003  Michal Rokos <m.rokos@sh.cvut.cz>
 * All rights reserved.
 */
/*
 * This program is licenced under the same licence as Ruby.
 * (See the file 'LICENCE'.)
 */
#if !defined(_OSSL_RUBY_MISSING_H_)
#define _OSSL_RUBY_MISSING_H_

#define rb_define_copy_func(klass, func) \
	rb_define_method((klass), "initialize_copy", (func), 1)


#ifndef GetReadFile
#define FPTR_TO_FD(fptr) ((fptr)->fd)
#else
#define FPTR_TO_FD(fptr) (fileno(GetReadFile(fptr)))
#endif

#ifndef HAVE_RB_IO_T
#define rb_io_t OpenFile
#endif

#endif /* _OSSL_RUBY_MISSING_H_ */
