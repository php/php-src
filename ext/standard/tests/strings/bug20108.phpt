--TEST--
Segfault on prinft statement Bug #20108
--SKIPIF--
--FILE--
<?php $a = "boo"; printf("%580.58s\n", $a); ?>
--EXPECT--
I have no idea yet :)
