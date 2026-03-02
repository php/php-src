--TEST--
Bug GH-9750 011 (Generator memory leak when interrupted during argument evaluation)
--FILE--
<?php

function f() {
}

class C {
    function getClosure() {
        return function () {
            return $this;
        };
    }
    function __destruct() {
        echo __METHOD__, "\n";
    }
}

$gen = function ($c) use (&$gen) {
    $c($gen, yield);
};

$gen = $gen((new C())->getClosure());

foreach ($gen as $value) {
    break;
}

$gen = null;

gc_collect_cycles();

?>
==DONE==
--EXPECT--
C::__destruct
==DONE==
