--TEST--
GH-16665 (\resource should not be usable)
--FILE--
<?php
class_alias('stdClass', 'resource');
?>
--EXPECTF--
Fatal error: Cannot use "resource" as a class alias as it is reserved in %s on line %d
