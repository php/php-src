Oniguruma
=========
## **This project ended on April 24, 2025.**

## **The only open source software attacked on Google search in Japan.** [(Issue #234)](https://github.com/kkos/oniguruma/issues/234)


https://github.com/kkos/oniguruma

Oniguruma is a modern and flexible regular expressions library. It
encompasses features from different regular expression implementations
that traditionally exist in different languages.

Character encoding can be specified per regular expression object.

Supported character encodings:

  ASCII, UTF-8, UTF-16BE, UTF-16LE, UTF-32BE, UTF-32LE,
  EUC-JP, EUC-TW, EUC-KR, EUC-CN,
  Shift_JIS, Big5, GB18030, KOI8-R, CP1251,
  ISO-8859-1, ISO-8859-2, ISO-8859-3, ISO-8859-4, ISO-8859-5,
  ISO-8859-6, ISO-8859-7, ISO-8859-8, ISO-8859-9, ISO-8859-10,
  ISO-8859-11, ISO-8859-13, ISO-8859-14, ISO-8859-15, ISO-8859-16

* GB18030: contributed by KUBO Takehiro
* CP1251:  contributed by Byte
* doc/SYNTAX.md: contributed by seanofw
* doc/onig_syn_md.c: tonco-miyazawa


Notice (from 6.9.6)
-------------------
When using configure script, if you have the POSIX API enabled in an earlier version (disabled by default in 6.9.5) and you need application binary compatibility with the POSIX API, specify "--enable-binary-compatible-posix-api=yes" instead of "--enable-posix-api=yes". Starting in 6.9.6, "--enable-posix-api=yes" only supports source-level compatibility for 6.9.5 and earlier about POSIX API. (Issue #210)


Master branch
-------------
* Unicode property \pC, \pL, \pM, \pN, \pP, \pS, \pZ


Version 6.9.10
--------------
* Update Unicode version 16.0
* Add new operator (*SKIP)    (PR#299)
* Fixed: ONIG_SYN_CONTEXT_INDEP_REPEAT_OPS not working for ^* pattern (Issue #292)


Version 6.9.9
-------------
* Update Unicode version 15.1.0
* NEW API: ONIG_OPTION_MATCH_WHOLE_STRING
* Fixed: (?I) option was not enabled for character classes (Issue #264).
* Changed specification to check for incorrect POSIX bracket (Issue #253).
* Changed [[:punct:]] in Unicode encodings to be compatible with POSIX definition. (Issue #268)
* Fixed: ONIG_OPTION_FIND_LONGEST behavior


Version 6.9.8
-------------
* Update Unicode version 14.0.0
* Whole options
    * (?C) : ONIG_OPTION_DONT_CAPTURE_GROUP
    * (?I) : ONIG_OPTION_IGNORECASE_IS_ASCII
    * (?L) : ONIG_OPTION_FIND_LONGEST
* Fixed some problems found by OSS-Fuzz


Version 6.9.7
-------------
* NEW API: ONIG_OPTION_CALLBACK_EACH_MATCH
* NEW API: ONIG_OPTION_IGNORECASE_IS_ASCII
* NEW API: ONIG_SYNTAX_PYTHON
* Fixed some problems found by OSS-Fuzz


Version 6.9.6
-------------
* NEW: configure option --enable-binary-compatible-posix-api=[yes/no]
* NEW API: Limiting the maximum number of calls of subexp-call
* NEW API: ONIG_OPTION_NOT_BEGIN_STRING / NOT_END_STRING / NOT_BEGIN_POSITION
* Fixed behavior of ONIG_OPTION_NOTBOL / NOTEOL
* Fixed many problems found by OSS-Fuzz
* Fixed many problems found by Coverity
* Fixed CVE-2020-26159 (This turned out not to be a problem later. #221)
* Under cygwin and mingw, generate and install the libonig.def file (Issue #220)


License
-------

  BSD license.


Install
-------

### Case 1: Linux distribution packages

   * Fedora:         `dnf install oniguruma-devel`
   * RHEL/CentOS:    `yum install oniguruma`
   * Debian/Ubuntu:  `apt install libonig5`
   * Arch:           `pacman -S oniguruma`
   * openSUSE:       `zypper install oniguruma`

### Case 2: Manual compilation on Linux, Unix, and Cygwin platform

   1. autoreconf -vfi   (* case: configure script is not found.)

   2. ./configure
   3. make
   4. make install

   * uninstall

     make uninstall

   * configuration check

     onig-config --cflags
     onig-config --libs
     onig-config --prefix
     onig-config --exec-prefix



### Case 3: Windows 64/32bit platform (Visual Studio)

   * build library

      .\make_win.bat

      onig_s.lib:  static link library
      onig.dll:    dynamic link library

   * make test programs

      .\make_win.bat all-test


Alternatively, you can build and install oniguruma using [vcpkg](https://github.com/microsoft/vcpkg/) dependency manager:

   1. git clone https://github.com/Microsoft/vcpkg.git
   2. cd vcpkg
   3. ./bootstrap-vcpkg.bat
   4. ./vcpkg integrate install
   5. ./vcpkg install oniguruma

The oniguruma port in vcpkg is kept up to date by microsoft team members and community contributors.
If the version is out of date, please [create an issue or pull request](https://github.com/Microsoft/vcpkg) on the vcpkg repository.

Regular Expressions
-------------------

  See [doc/RE](doc/RE) or [doc/RE.ja](doc/RE.ja) for Japanese.


Usage
-----

  Include oniguruma.h in your program. (Oniguruma API)
  See doc/API for Oniguruma API.

  If you want to disable UChar type (== unsigned char) definition
  in oniguruma.h, define ONIG_ESCAPE_UCHAR_COLLISION and then
  include oniguruma.h.

  If you want to disable regex_t type definition in oniguruma.h,
  define ONIG_ESCAPE_REGEX_T_COLLISION and then include oniguruma.h.

  Example of the compiling/linking command line in Unix or Cygwin,
  (prefix == /usr/local case)

    cc sample.c -L/usr/local/lib -lonig


  If you want to use static link library(onig_s.lib) in Win32,
  add option -DONIG_EXTERN=extern to C compiler.



Sample Programs
---------------

|File                  |Description                               |
|:---------------------|:-----------------------------------------|
|sample/callout.c      |example of callouts                       |
|sample/count.c        |example of built-in callout *COUNT        |
|sample/echo.c         |example of user defined callouts of name  |
|sample/encode.c       |example of some encodings                 |
|sample/listcap.c      |example of the capture history            |
|sample/names.c        |example of the named group callback       |
|sample/posix.c        |POSIX API sample                          |
|sample/regset.c       |example of using RegSet API               |
|sample/scan.c         |example of using onig_scan()              |
|sample/simple.c       |example of the minimum (Oniguruma API)    |
|sample/sql.c          |example of the variable meta characters   |
|sample/user_property.c|example of user defined Unicode property  |


Test Programs

|File               |Description                            |
|:------------------|:--------------------------------------|
|sample/syntax.c    |Perl, Java and ASIS syntax test.       |
|sample/crnl.c      |--enable-crnl-as-line-terminator test  |



Source Files
------------

|File               |Description                                             |
|:------------------|:-------------------------------------------------------|
|oniguruma.h        |Oniguruma API header file (public)                      |
|onig-config.in     |configuration check program template                    |
|regenc.h           |character encodings framework header file               |
|regint.h           |internal definitions                                    |
|regparse.h         |internal definitions for regparse.c and regcomp.c       |
|regcomp.c          |compiling and optimization functions                    |
|regenc.c           |character encodings framework                           |
|regerror.c         |error message function                                  |
|regext.c           |extended API functions (deluxe version API)             |
|regexec.c          |search and match functions                              |
|regparse.c         |parsing functions.                                      |
|regsyntax.c        |pattern syntax functions and built-in syntax definitions|
|regtrav.c          |capture history tree data traverse functions            |
|regversion.c       |version info function                                   |
|st.h               |hash table functions header file                        |
|st.c               |hash table functions                                    |
|oniggnu.h          |GNU regex API header file (public)                      |
|reggnu.c           |GNU regex API functions                                 |
|onigposix.h        |POSIX API header file (public)                          |
|regposerr.c        |POSIX error message function                            |
|regposix.c         |POSIX API functions                                     |
|mktable.c          |character type table generator                          |
|ascii.c            |ASCII encoding                                          |
|euc_jp.c           |EUC-JP encoding                                         |
|euc_tw.c           |EUC-TW encoding                                         |
|euc_kr.c           |EUC-KR, EUC-CN encoding                                 |
|sjis.c             |Shift_JIS encoding                                      |
|big5.c             |Big5      encoding                                      |
|gb18030.c          |GB18030   encoding                                      |
|koi8.c             |KOI8      encoding                                      |
|koi8_r.c           |KOI8-R    encoding                                      |
|cp1251.c           |CP1251    encoding                                      |
|iso8859_1.c        |ISO-8859-1 (Latin-1)                                    |
|iso8859_2.c        |ISO-8859-2 (Latin-2)                                    |
|iso8859_3.c        |ISO-8859-3 (Latin-3)                                    |
|iso8859_4.c        |ISO-8859-4 (Latin-4)                                    |
|iso8859_5.c        |ISO-8859-5 (Cyrillic)                                   |
|iso8859_6.c        |ISO-8859-6 (Arabic)                                     |
|iso8859_7.c        |ISO-8859-7 (Greek)                                      |
|iso8859_8.c        |ISO-8859-8 (Hebrew)                                     |
|iso8859_9.c        |ISO-8859-9 (Latin-5 or Turkish)                         |
|iso8859_10.c       |ISO-8859-10 (Latin-6 or Nordic)                         |
|iso8859_11.c       |ISO-8859-11 (Thai)                                      |
|iso8859_13.c       |ISO-8859-13 (Latin-7 or Baltic Rim)                     |
|iso8859_14.c       |ISO-8859-14 (Latin-8 or Celtic)                         |
|iso8859_15.c       |ISO-8859-15 (Latin-9 or West European with Euro)        |
|iso8859_16.c       |ISO-8859-16 (Latin-10)                                  |
|utf8.c             |UTF-8    encoding                                       |
|utf16_be.c         |UTF-16BE encoding                                       |
|utf16_le.c         |UTF-16LE encoding                                       |
|utf32_be.c         |UTF-32BE encoding                                       |
|utf32_le.c         |UTF-32LE encoding                                       |
|unicode.c          |common codes of Unicode encoding                        |
|unicode_fold_data.c|Unicode folding data                                    |
|windows/testc.c    |Test program for Windows (VC++)                        |
