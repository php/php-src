--TEST--
spl_object_hash(): is deprecated
--FILE--
<?php

$o = new stdClass();
var_dump(spl_object_hash($o));

?>
--EXPECTF--
Deprecated: Function spl_object_hash() is deprecated since 8.6, consider using spl_object_id() instead in %s on line %d
string(32) "00000000000000010000000000000000"
