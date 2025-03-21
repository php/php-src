--TEST--
`never` parameter types - can only be applied to class methods
--FILE--
<?php

function demo(never $unused) {}
?>
--EXPECTF--
Fatal error: never cannot be used as a parameter type for functions in %s on line %d
