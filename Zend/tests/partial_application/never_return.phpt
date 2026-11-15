--TEST--
PFA can be made from never function
--FILE--
<?php

function neverReturns($msg): never {
	throw new Error($msg);
}

var_dump(neverReturns(?) instanceof Closure);

?>
--EXPECT--
bool(true)
