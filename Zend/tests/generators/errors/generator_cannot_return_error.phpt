--TEST--
Generators cannot return values
--FILE--
<?php

function gen() {
    yield;
	return $abc;
}

?>
--EXPECTF--
Fatal error: Generators cannot return values using "return" in %s on line 5
