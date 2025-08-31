--TEST--
Bug GH-9750 008 (Generator memory leak when interrupted during argument evaluation)
--SKIPIF--
<?php
if (version_compare(PHP_VERSION, '8.1', '<')) { die('skip Broken on PHP < 8.1'); }
?>
--FILE--
<?php

class C {
    function __destruct() {
        echo __METHOD__, "\n";
    }
}

class D {
    function __destruct() {
        echo __METHOD__, "\n";
    }
}

$gen = function ($c) use (&$gen) {
    new D($gen, yield);
};

$gen = $gen(new C());

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
