--TEST--
PFA compilation warnings are recorded and replayed
--FILE--
<?php

function f(_ $a) {}

$f = f(?);

?>
--EXPECTF--
Deprecated: Using "_" as a type name is deprecated since 8.4 in %s on line 3

Deprecated: Using "_" as a type name is deprecated since 8.4 in %s on line 5
