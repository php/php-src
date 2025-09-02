--TEST--
Test unserialize() with the 'S' format emits a deprecation.
--FILE--
<?php

var_dump(unserialize('S:1:"e";'));
var_dump(unserialize('S:1:"\65";'));

?>
--EXPECTF--
Deprecated: unserialize(): Unserializing the 'S' format is deprecated in %s on line %d
string(1) "e"

Deprecated: unserialize(): Unserializing the 'S' format is deprecated in %s on line %d
string(1) "e"
