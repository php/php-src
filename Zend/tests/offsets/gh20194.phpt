--TEST--
GH-20194: Using null as an array offset does not emit deprecation when resolved at compile time
--FILE--
<?php

$a = [null => 1];

echo $a[null];

?>
--EXPECTF--
Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d

Deprecated: Using null as an array offset is deprecated, use an empty string instead in %s on line %d
1
