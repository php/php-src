--TEST--
mb_detect_encoding()
--SKIPIF--
<?php include('skipif.inc'); ?>
--POST--
--GET--
--FILE--
<?php include('017.inc'); ?>
--EXPECT--
== BASIC TEST ==
SJIS: SJIS
JIS: JIS
EUC-JP: EUC-JP
EUC-JP: EUC-JP
== ARRAY ENCODING LIST ==
JIS: UTF-8
EUC-JP: EUC-JP
SJIS: SJIS
== DETECT ORDER ==
JIS: JIS
EUC-JP: EUC-JP
SJIS: SJIS
== INVALID PARAMETER ==
INT: EUC-JP
EUC-JP: EUC-JP
ERR: Warning
BAD: EUC-JP
ERR: Warning
MP: 

