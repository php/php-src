--TEST--
chr() with out of range values
--FILE--
<?php

var_dump("\xFF" == chr(-1));
var_dump("\0" == chr(256));

?>
--EXPECTF--
Deprecated: chr(): Providing a value not in-between 0 and 255 is deprecated, this is because a byte value must be in the [0, 255] interval. The value used will be constrained using % 256 in %s on line 3
bool(true)

Deprecated: chr(): Providing a value not in-between 0 and 255 is deprecated, this is because a byte value must be in the [0, 255] interval. The value used will be constrained using % 256 in %s on line 4
bool(true)
