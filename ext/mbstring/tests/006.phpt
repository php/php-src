--TEST--
mb_substitute_character()  
--SKIPIF--
<?php include('skipif.inc'); ?>
--POST--
--GET--
--FILE--
<?php include('006.inc'); ?>
--EXPECT--
OK_UTF
12307
OK_LONG
long
OK_NONE
none
== INVALID PARAMETER ==
ERR: Warning
OK_BAD_NAME

