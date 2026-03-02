--TEST--
GC on running generator
--FILE--
<?php

function gen() {
    yield;
    // Trigger GC while $v is being reassigned.
    $ary = [new stdClass, new stdClass, new stdClass];
    $ary[0]->foo = $ary;
    foreach ($ary as &$v) { }
}

for ($i = 0; $i < 10000; $i++) {
    // Make sure gen is registered as a GC root.
    $gen = gen();
    $gen2 = $gen;
    unset($gen);
    foreach ($gen2 as $v) {}
}

?>
===DONE===
--EXPECT--
===DONE===
