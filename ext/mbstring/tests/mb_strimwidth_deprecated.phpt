--TEST--
mb_strimwidth() is deprecated with negative width
--EXTENSIONS--
mbstring
--FILE--
<?php
var_dump(mb_strimwidth("some string", 1, -2, '...', 'ASCII'));
?>
--EXPECTF--
Deprecated: mb_strimwidth(): passing a negative integer to argument #3 ($width) is deprecated in %s on line %d
string(8) "ome s..."
