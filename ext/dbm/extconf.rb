require 'mkmf'

dir_config("dbm")

if dblib = with_config("dbm-type", nil)
  dblib = dblib.split(/[ ,]+/)
else
  dblib = %w(libc db db2 db1 db5 db4 db3 gdbm_compat gdbm qdbm)
end

headers = {
  "libc" => ["ndbm.h"], # 4.3BSD original ndbm, Berkeley DB 1 in 4.4BSD libc.
  "db" => ["db.h"],
  "db1" => ["db1/ndbm.h", "db1.h", "ndbm.h"],
  "db2" => ["db2/db.h", "db2.h", "db.h"],
  "db3" => ["db3/db.h", "db3.h", "db.h"],
  "db4" => ["db4/db.h", "db4.h", "db.h"],
  "db5" => ["db5/db.h", "db5.h", "db.h"],
  "gdbm_compat" => ["gdbm-ndbm.h", "gdbm/ndbm.h", "ndbm.h"], # GDBM since 1.8.1
  "gdbm" => ["gdbm-ndbm.h", "gdbm/ndbm.h", "ndbm.h"], # GDBM until 1.8.0
  "qdbm" => ["qdbm/relic.h", "relic.h"],
}

class << headers
  attr_accessor :found
  attr_accessor :defs
end
headers.found = []
headers.defs = nil

def headers.db_check(db, hdr)
  old_libs = $libs.dup
  old_defs = $defs.dup
  result = db_check2(db, hdr)
  if !result
    $libs = old_libs
    $defs = old_defs
  end
  result
end

def have_declared_libvar(var, headers = nil, opt = "", &b)
  checking_for checking_message([*var].compact.join(' '), headers, opt) do
    try_declared_libvar(var, headers, opt, &b)
  end
end

def try_declared_libvar(var, headers = nil, opt = "", &b)
  if try_link(<<"SRC", opt, &b)
#{cpp_include(headers)}
/*top*/
int main(int argc, char *argv[]) {
  void *conftest_var = &#{var};
  return 0;
}
SRC
    $defs.push(format("-DHAVE_DECLARED_LIBVAR_%s", var.tr_cpp))
    true
  else
    false
  end
end

def have_undeclared_libvar(var, headers = nil, opt = "", &b)
  checking_for checking_message([*var].compact.join(' '), headers, opt) do
    try_undeclared_libvar(var, headers, opt, &b)
  end
end

def try_undeclared_libvar(var, headers = nil, opt = "", &b)
  var, type = *var
  if try_link(<<"SRC", opt, &b)
#{cpp_include(headers)}
/*top*/
int main(int argc, char *argv[]) {
  typedef #{type || 'int'} conftest_type;
  extern conftest_type #{var};
  conftest_type *conftest_var = &#{var};
  return 0;
}
SRC
    $defs.push(format("-DHAVE_UNDECLARED_LIBVAR_%s", var.tr_cpp))
    true
  else
    false
  end
end

def have_empty_macro_dbm_clearerr(headers = nil, opt = "", &b)
  checking_for checking_message('empty macro of dbm_clearerr(foobarbaz)',
                                headers, opt) do
    try_toplevel('dbm_clearerr(foobarbaz)', headers, opt, &b)
  end
end

def try_toplevel(src, headers = nil, opt = "", &b)
  if try_compile(<<"SRC", opt, &b)
#{cpp_include(headers)}
/*top*/
#{src}
SRC
    true
  else
    false
  end
end


def headers.db_check2(db, hdr)
  $defs.push(%{-DRUBYDBM_DBM_HEADER='"#{hdr}"'})
  $defs.push(%{-DRUBYDBM_DBM_TYPE='"#{db}"'})

  hsearch = nil

  case db
  when /^db[2-5]?$/
    hsearch = "-DDB_DBM_HSEARCH"
  when "gdbm_compat"
    have_library("gdbm") or return false
  end

  if !have_type("DBM", hdr, hsearch)
    return false
  end

  # 'libc' means ndbm is provided by libc.
  # 4.3BSD original ndbm is contained in libc.
  # 4.4BSD (and its derivatives such as NetBSD) contains Berkeley DB 1 in libc.
  if !(db == 'libc' ? have_func('dbm_open("", 0, 0)', hdr, hsearch) :
                      have_library(db, 'dbm_open("", 0, 0)', hdr, hsearch))
    return false
  end

  # Skip a mismatch of Berkeley DB's ndbm.h and old GDBM library.
  #
  # dbm_clearerr() should be available for any ndbm implementation.
  # It is available since the original (4.3BSD) ndbm and standardized by POSIX.
  #
  # However "can't resolve symbol 'dbm_clearerr'" problem may be caused by
  # header/library mismatch: Berkeley DB ndbm.h and GDBM library until 1.8.3.
  # GDBM (until 1.8.3) provides dbm_clearerr() as a empty macro in the header
  # and the library don't provide dbm_clearerr().
  # Berkeley DB provides dbm_clearerr() as a usual function.
  # So Berkeley DB header with GDBM library causes the problem.
  #
  if !have_func('dbm_clearerr((DBM *)0)', hdr, hsearch)
    return false
  end

  # Berkeley DB's ndbm.h (since 1.85 at least) defines DBM_SUFFIX.
  # Note that _DB_H_ is not defined on Mac OS X because 
  # it uses Berkeley DB 1 but ndbm.h doesn't include db.h.
  have_db_header = have_macro('DBM_SUFFIX', hdr, hsearch)

  # Old GDBM's ndbm.h, until 1.8.3, defines dbm_clearerr as a macro which
  # expands to no tokens.
  have_gdbm_header1 = have_empty_macro_dbm_clearerr(hdr, hsearch)

  # Recent GDBM's ndbm.h, since 1.9, includes gdbm.h and it defines _GDBM_H_.
  # ndbm compatibility layer of GDBM is provided by libgdbm (until 1.8.0)
  # and libgdbm_compat (since 1.8.1).
  have_gdbm_header2 = have_macro('_GDBM_H_', hdr, hsearch)

  # 4.3BSD's ndbm.h defines _DBM_IOERR.
  # The original ndbm is provided by libc in 4.3BSD.
  have_ndbm_header = have_macro('_DBM_IOERR', hdr, hsearch)

  # GDBM provides ndbm functions in libgdbm_compat since GDBM 1.8.1.
  # GDBM's ndbm.h defines _GDBM_H_ since GDBM 1.9.
  # If _GDBM_H_ is defined, 'gdbm_compat' is required and reject 'gdbm'.
  if have_gdbm_header2 && db == 'gdbm'
    return false
  end

  if have_db_header
    $defs.push('-DRUBYDBM_DB_HEADER')
  end

  have_gdbm_header = have_gdbm_header1 | have_gdbm_header2
  if have_gdbm_header
    $defs.push('-DRUBYDBM_GDBM_HEADER')
  end

  # ndbm.h is provided by the original (4.3BSD) ndbm,
  # Berkeley DB 1 in libc of 4.4BSD and
  # ndbm compatibility layer of GDBM.
  # So, try to check header/library mismatch.
  #
  # Several (possibly historical) distributions provides libndbm.
  # It may be Berkeley DB, GDBM or 4.3BSD ndbm.
  # So mismatch check is not performed for that.
  # Note that libndbm is searched only when --with-dbm-type=ndbm is
  # given for configure.
  #
  if hdr == 'ndbm.h' && db != 'libc' && db != 'ndbm'
    if /\Adb\d?\z/ !~ db && have_db_header
      return false
    end

    if /\Agdbm/ !~ db && have_gdbm_header
      return false
    end
    
    if have_ndbm_header
      return false
    end
  end

  # Berkeley DB
  have_func('db_version((int *)0, (int *)0, (int *)0)', hdr, hsearch)

  # GDBM
  have_gdbm_version = have_declared_libvar("gdbm_version", hdr, hsearch)
  # gdbm_version is available since very old version (GDBM 1.5 at least).
  # However it is not declared by ndbm.h until GDBM 1.8.3.
  # We can't include both ndbm.h and gdbm.h because they both define datum type.
  # ndbm.h includes gdbm.h and gdbm_version is declared since GDBM 1.9.
  have_gdbm_version |= have_undeclared_libvar(["gdbm_version", "char *"], hdr, hsearch)

  # QDBM
  have_var("dpversion", hdr, hsearch)

  # detect mismatch between GDBM header and other library.
  # If GDBM header is included, GDBM library should be linked.
  if have_gdbm_header && !have_gdbm_version
    return false
  end

  # DBC type is required to disable error messages by Berkeley DB 2 or later.
  if have_db_header
    have_type("DBC", hdr, hsearch)
  end

  if hsearch
    $defs << hsearch
    @defs = hsearch
  end
  $defs << '-DDBM_HDR="<'+hdr+'>"'
  @found << hdr

  puts "header: #{hdr}"
  puts "library: #{db}"

  true
end

if dblib.any? {|db| headers.fetch(db, ["ndbm.h"]).any? {|hdr| headers.db_check(db, hdr) } }
  have_header("cdefs.h")
  have_header("sys/cdefs.h")
  have_func("dbm_pagfno((DBM *)0)", headers.found, headers.defs)
  have_func("dbm_dirfno((DBM *)0)", headers.found, headers.defs)
  convertible_int("datum.dsize", headers.found, headers.defs)
  create_makefile("dbm")
end
