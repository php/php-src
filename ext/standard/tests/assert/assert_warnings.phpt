--TEST--
assert(): warnings with no descriptions.
--INI--
assert.active = 1
assert.warning = 1
assert.bail = 0
assert.exception=0
--FILE--
<?php
assert(0, null);
?>
--EXPECTF--
Deprecated: PHP Startup: assert.exception INI setting is deprecated in Unknown on line 0

Warning: assert(): Assertion failed in %s on line %d
