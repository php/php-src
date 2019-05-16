--TEST--
Bug #68545 NULL pointer dereference in unserialize.c:var_push_dtor
--FILE--
<?php
var_dump(unserialize('a:6:{a:6:{s:3:"322";s:3:"bar";s:3:"bar";s:3:"foo";a:6:{a:6:{s:3:"322";s:3:"bar";s:3:"bar";s:3:"foo";s:3:"bar";a:6:{a:6:{s:3:"322";s:3:"bar";s:3:"bar";s:3:"foo";a:6:{a:6:{s:3:"322";s:3:"bar";s:3:"b22";s:3:"bar";s:3:"bar";s:3:"foo";s:3:"bar";a:6:{a:6:{s:3:"322";s:3:"bar";s:3:"bar";s:3:"foo";s:3:"bar";s:3:"bar";'));
?>
===DONE===
--EXPECTF--
Notice: unserialize(): Error at offset %d of %d bytes in %sbug68545.php on line %d
bool(false)
===DONE===
