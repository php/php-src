--TEST--
iconv test
--SKIPIF--
<?php include('skipif.inc'); ?>
--POST--
--GET--
--FILE--
<?php include('002.inc'); ?>
--EXPECT--
Expected output:
&#97;&#98;&#99;&#100;
abcd
