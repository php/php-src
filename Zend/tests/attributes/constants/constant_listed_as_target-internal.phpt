--TEST--
Constants listed in valid targets when used wrong (internal attribute)
--FILE--
<?php

function demo(
	#[Deprecated] $v
) {}

?>
--EXPECTF--
Fatal error: Attribute "Deprecated" cannot target parameter (allowed targets: class, function, method, class constant, constant) in %s on line %d
