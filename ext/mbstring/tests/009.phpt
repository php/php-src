--TEST--
mb_strlen()
--SKIPIF--
<?php include('skipif.inc'); ?>
--POST--
--GET--
--FILE--
<?php include('009.inc'); ?>
--EXPECT--
== ASCII ==
40
40
== EUC-JP ==
43
43
== SJIS ==
43
43
== JIS ==
43
43
== UTF-8 ==
43
43
== WRONG PARAMETERS ==
ERR: Notice

ERR: Notice

ERR: Warning



