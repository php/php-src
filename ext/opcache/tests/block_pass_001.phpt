--TEST--
Block pass: Don't suppress undefined variable notice
--FILE--
<?php
(bool) (true ? $x : $y);
?>
--EXPECTF--
Notice: Undefined variable: x in %s on line %d
