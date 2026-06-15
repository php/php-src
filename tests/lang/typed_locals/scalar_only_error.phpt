--TEST--
Typed local variables: non-scalar type triggers compile error
--FILE--
<?php
stdClass $o = new stdClass();
?>
--EXPECTF--
Fatal error: Typed local variables support only scalar types (int, float, string, bool) in this version in %s on line %d
