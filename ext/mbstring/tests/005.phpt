--TEST--
mb_detect_order()  
--SKIPIF--
<?php include('skipif.inc'); ?>
--POST--
--GET--
--FILE--
<?php include('005.inc'); ?>
--EXPECT--
OK_AUTO
ASCII, JIS, UTF-8, EUC-JP, SJIS
OK_STR
SJIS, EUC-JP, JIS, UTF-8
OK_ARRAY
ASCII, JIS, EUC-JP, UTF-8
== INVALID PARAMETER ==
OK_BAD_STR
ASCII, JIS, EUC-JP, UTF-8
OK_BAD_ARRAY
ASCII, JIS, EUC-JP, UTF-8

