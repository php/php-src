--TEST--
mb_internal_encoding() test 
--SKIPIF--
<?php include('skipif.inc'); ?>
--POST--
--GET--
--FILE--
<?php include('002.inc'); ?>
--EXPECT--
OK_EUC-JP_SET
EUC-JP
OK_UTF-8_SET
UTF-8
OK_ASCII_SET
ASCII
== INVALID PARAMETER ==
ERR: Warning
OK_BAD_SET
ASCII
ERR: Notice
ERR: Warning
OK_BAD_ARY_SET
ASCII
ERR: Notice
ERR: Warning
OK_BAD_OBJ_SET
ASCII

