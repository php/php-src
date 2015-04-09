--TEST--
It is not possible to resume an already running generator
--FILE--
<?php

function gen() {
    $gen = yield;
    try {
	    $gen->next();
	} catch (EngineException $e) {
		echo "\nException: " . $e->getMessage() . "\n";
	}
	$gen->next();
}

$gen = gen();
$gen->send($gen);
$gen->next();

?>
--EXPECTF--
Exception: Cannot resume an already running generator

Fatal error: Cannot resume an already running generator in %s on line %d
