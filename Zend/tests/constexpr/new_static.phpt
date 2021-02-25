--TEST--
Static in new is not supported
--FILE--
<?php

const FOO = new static;

?>
--EXPECTF--
Fatal error: "static" is not allowed in compile-time constants in %s on line %d
