# libmbfl

This is libmbfl, a streamable multibyte character code filter and converter
library, written by Shigeru Kanemoto.

The original version of libmbfl is developed and distributed at
https://github.com/moriyoshi/libmbfl under the LGPL 2.1 license. See the LICENSE
file for licensing information.

The libmbfl library is bundled with PHP as a fork of the original repository and
is not in sync with the [upstream](https://github.com/moriyoshi/libmbfl). As
such, the libmbfl directory is directly modified in the php-src repository.

## Changelog

### October 2017

* Since 2017, it is forked and bundled in the php-src repository. For the list
  of changes related to PHP see the PHP NEWS change logs.

### Version 1.3.2 August 20, 2011

* Added JISX-0213:2004 based encoding : Shift_JIS-2004, EUC-JP-2004,
  ISO-2022-JP-2004 (rui).
* Added gb18030 encoding (rui).
* Added CP950 with user user defined area based on Big5 (rui).
* Added mapping for user defined character area to CP936 (rui).
* Added UTF-8-Mobile to support the pictogram characters defined by mobile phone
  carrier in Japan (rui).

### Version 1.3.1 August 5, 2011

* Added check for invalid/obsolete utf-8 encoding (rui).

### Version 1.3.0 August 1, 2011

* Added encoding conversion between Shift_JIS and Unicode (6.0 or PUA) for
  pictogram characters defined by mobile phone carrier in Japan (rui).

  [Detailed info](https://github.com/hirokawa/libmbfl/wiki/Introduction-to-libmbflhttps:--github.com-hirokawa-libmbfl-wiki-Introduction-to-libmbfl-_edit%23)

* Fixed encoding conversion of cp5022x for user defined area (rui).
* Added MacJapanese (SJIS-mac) for legacy encoding support (rui).
* Backport from PHP 5.2 (rui).

### Version 1.1.0 March 02, 2010

* Added cp5022x encoding (moriyoshi)
* Added ISO-2022-JP-MS (moriyoshi)
* Moved to github.com from sourceforge.jp (moriyoshi)

### Earlier versions

* 1998/11/10 sgk implementation in C++
* Rewriting with sgk C 1999/4/25.
* 1999/4/26 Implemented sgk input filter. Add filter while estimating kanji
  code.
* 1999/6 Unicode support.
* 1999/6/22 Changed sgk license to LGPL.

## Credits

Marcus Boerger <helly@php.net>
Hayk Chamyan <hamshen@gmail.com>
Wez Furlong <wez@thebrainroom.com>
Rui Hirokawa <hirokawa@php.net>
Shigeru Kanemoto <sgk@happysize.co.jp>
U. Kenkichi <kenkichi@axes.co.jp>
Moriyoshi Koizumi <moriyoshi@php.net>
Hironori Sato <satoh@jpnnet.com>
Tsukada Takuya <tsukada@fminn.nagano.nagano.jp>
Tateyama <tateyan@amy.hi-ho.ne.jp>
Den V. Tsopa <tdv@edisoft.ru>
Maksym Veremeyenko <verem@m1stereo.tv>
Haluk AKIN <halukakin@gmail.com>
