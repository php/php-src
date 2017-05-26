--TEST--
The (unset) cast is deprecated
--FILE--
<?php

$x = 1;
var_dump((unset) $x);
var_dump($x);

?>
--EXPECTF--
Deprecated: The (unset) cast is deprecated in %s on line %d
NULL
int(1)
