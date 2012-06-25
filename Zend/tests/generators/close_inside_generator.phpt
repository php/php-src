--TEST--
Calling close() during the exectution of the generator
--FILE--
<?php

function *gen() {
    /* Pass the generator object itself in */
    $gen = yield;

    /* Close generator while it is currently running */
    $gen->close();

	echo "Still running";
}

$gen = gen();
$gen->send($gen);

?>
--EXPECTF--
Warning: A generator cannot be closed while it is running in %s on line %d
Still running
