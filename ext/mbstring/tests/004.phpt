--TEST--
mb_http_output()  
--SKIPIF--
<?php include('skipif.inc'); ?>
--POST--
--GET--
--FILE--
<?php include('004.inc'); ?>
--EXPECT--
OK_ASCII_SET
ASCII
OK_SJIS_SET
SJIS
OK_JIS_SET
JIS
OK_UTF-8_SET
UTF-8
OK_EUC-JP_SET
EUC-JP
== INVALID PARAMETER ==
ERR: Warning
OK_BAD_SET
EUC-JP
ERR: Notice
ERR: Warning
OK_BAD_ARY_SET
EUC-JP
ERR: Notice
ERR: Warning
OK_BAD_OBJ_SET
EUC-JP

