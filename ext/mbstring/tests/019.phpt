--TEST--
mb_strlen()
--SKIPIF--
<?php include('skipif.inc'); ?>
--POST--
--GET--
--FILE--
<?php include('019.inc'); ?>
--EXPECT--
== ASCII ==
ERR: Warning

40
== EUC-JP ==
ERR: Warning

72
== SJIS ==
ERR: Warning

72
== JIS ==
ERR: Warning

121
== UTF-8 ==
ERR: Warning

174
== WRONG PARAMETERS ==
ERR: Notice
5
ERR: Notice
6
ERR: Warning



