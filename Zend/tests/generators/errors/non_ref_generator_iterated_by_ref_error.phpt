--TEST--
Non-ref generators cannot be iterated by-ref
--FILE--
<?php

function *gen() { }

$gen = gen();
foreach ($gen as &$value) { }

?>
--EXPECTF--
Fatal error: You can only iterate a generator by-reference if it declared that it yields by-reference in %s on line %d
