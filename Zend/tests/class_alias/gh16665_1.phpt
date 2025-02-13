--TEST--
GH-16665 (\array should not be usable)
--FILE--
<?php
class_alias('stdClass', 'array');
?>
--EXPECTF--
Fatal error: Cannot use "array" as a class alias as it is reserved in %s on line %d
