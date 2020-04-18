--TEST--
The (unset) cast is removed
--FILE--
<?php

$x = 1;
var_dump((unset) $x);
var_dump($x);

?>
--EXPECTF--
Fatal error: The (unset) cast is no longer supported in %s on line %d
