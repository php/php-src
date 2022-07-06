--TEST--
Disallow tail empty elements in normal arrays
--FILE--
<?php

var_dump([1, 2, ,]);

?>
--EXPECTF--
Fatal error: Cannot use empty array elements in arrays in %s on line %d
