--TEST--
OSS-Fuzz 60011 (Incorrect order of instruction with nullsafe operator)
--FILE--
<?php
[&$y]=$y->y?->y;
?>
--EXPECTF--
Fatal error: Uncaught Error: Attempt to modify property "y" on null in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
