--TEST--
Bug #37676 (Constant warnings)
--FILE--
<?php
const FOO = null;
const BAR = FOO[0][1];
?>
--EXPECTF--
Warning: Variable of type null does not accept array offsets in %s on line %d
