--TEST--
UCS4BE to ASCII
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php include('iconv002.inc'); ?>
--EXPECT--
&#97;&#98;&#99;&#100;
abcd
