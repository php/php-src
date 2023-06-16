--TEST--
Relative class types can be used for closures as it may be bound to a class
--FILE--
<?php

$fn1 = function($x): self {};
$fn2 = function($x): parent {};
$fn3 = function($x): static {};

?>
DONE
--EXPECT--
DONE
