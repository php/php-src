--TEST--
resource typehint: acceptable cases
--FILE--
<?php

function foo(resource $foo) {
    // okay
}

try {
	foo(true);
} catch (TypeError $e) {
	echo $e->getMessage();
}

--EXPECTF--
Argument 1 passed to foo() must be of the type resource, boolean given, called in %s on line %d

