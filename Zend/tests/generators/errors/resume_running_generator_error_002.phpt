--TEST--
Memory leak when resume an already running generator
--FILE--
<?php
function gen() {
    $g = yield;
    $g->send($g);
}
$gen = gen();
try {
    $gen->send($gen);
} catch (Throwable $e) {
    echo $e->getMessage() . "\n";	
}
?>
--EXPECT--
Cannot resume an already running generator
