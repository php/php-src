--TEST--
UCS4BE to ASCII
--SKIPIF--
<?php include('skipif.inc'); ?>
--POST--
--GET--
--FILE--
<?php include('002.inc'); ?>
--EXPECT--
&#97;&#98;&#99;&#100;
abcd
