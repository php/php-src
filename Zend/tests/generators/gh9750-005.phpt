--TEST--
Bug GH-9750 002 (Generator memory leak when interrupted during argument evaluation)
--FILE--
<?php

function f(...$x) {
}

class C {
    function __destruct() {
        echo __METHOD__, "\n";
    }
}

$gen = function ($c) use (&$gen) {
    f(b: $gen, c: yield);
};

$gen = $gen(new C());

foreach ($gen as $value) {
    break;
}

$gen = null;
$c = null;

gc_collect_cycles();

?>
==DONE==
--EXPECT--
C::__destruct
==DONE==
