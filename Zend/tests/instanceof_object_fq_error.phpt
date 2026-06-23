--TEST--
instanceof '\object' must produce a compile error (object must be unqualified)
--FILE--
<?php
$x = new stdClass;
var_dump($x instanceof \object);
?>
--EXPECTF--
Fatal error: Type declaration 'object' must be unqualified in %s on line %d
