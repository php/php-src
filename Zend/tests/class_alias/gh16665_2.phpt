--TEST--
GH-16665 (\callable should not be usable)
--FILE--
<?php
class_alias('stdClass', 'callable');
?>
--EXPECTF--
Fatal error: Cannot use "callable" as a class alias as it is reserved in %s on line %d
