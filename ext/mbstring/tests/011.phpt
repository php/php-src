--TEST--
mb_strrpos()
--SKIPIF--
<?php include('skipif.inc'); ?>
--POST--
--GET--
--FILE--
<?php include('011.inc'); ?>
--EXPECT--
== WITH ENCODING PARAMETER ==
34
30
33
30
OK_STR
OK_NEWLINE
== NO ENCODING PARAMETER ==
34
30
33
30
OK_STR
OK_NEWLINE
== INVALID PARAMETER TEST ==
ERR: Warning
OK_NULL
ERR: Notice
OK_ARRAY
ERR: Notice
OK_OBJECT

