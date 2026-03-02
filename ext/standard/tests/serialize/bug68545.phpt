--TEST--
Bug #68545 NULL pointer dereference in unserialize.c:var_push_dtor
--FILE--
<?php
var_dump(unserialize('a:6:{a:6:{s:3:"322";s:3:"bar";s:3:"bar";s:3:"foo";a:6:{a:6:{s:3:"322";s:3:"bar";s:3:"bar";s:3:"foo";s:3:"bar";a:6:{a:6:{s:3:"322";s:3:"bar";s:3:"bar";s:3:"foo";a:6:{a:6:{s:3:"322";s:3:"bar";s:3:"b22";s:3:"bar";s:3:"bar";s:3:"foo";s:3:"bar";a:6:{a:6:{s:3:"322";s:3:"bar";s:3:"bar";s:3:"foo";s:3:"bar";s:3:"bar";'));
?>
--EXPECTF--
Warning: unserialize(): Error at offset 10 of 310 bytes in %s on line %d
bool(false)
