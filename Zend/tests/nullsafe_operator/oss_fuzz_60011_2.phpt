--TEST--
oss-fuzz #60011 (Incorrect order of instruction with nullsafe operator)
--FILE--
<?php
[&$y]=$y?->y->y;
?>
--EXPECTF--
Fatal error: Cannot take reference of a nullsafe chain in %s on line %d
