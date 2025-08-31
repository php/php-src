--TEST--
Bug GH-9750 009 (Generator memory leak when interrupted during argument evaluation)
--FILE--
<?php

function f() {
}

class C {
    public function __construct(public mixed $x) {
    }
    public function __invoke() {
    }
    public function __destruct() {
        echo __METHOD__, "\n";
    }
}

$gen = function () use (&$gen) {
    (new C($gen))(yield);
};

$gen = $gen();

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
