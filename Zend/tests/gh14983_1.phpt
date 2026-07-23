--TEST--
GH-14983: Shutdown disregards guards active on bailout
--SKIPIF--
<?php if (getenv("SKIP_SLOW_TESTS")) die('skip slow test'); ?>
--FILE--
<?php

ini_set('max_execution_time', 1);
$loop = true;

class A {
    function __get($name) {
        global $loop;
        if ($loop) {
            while (true) {}
        }
        echo __METHOD__, "\n";
        return $this->{$name};
    }
}

global $a;
$a = new A;

function shutdown() {
    global $a;
    global $loop;
    $loop = false;
    var_dump($a->foo);
}

register_shutdown_function('shutdown');

$a->foo;

?>
--EXPECTF--
Fatal error: Maximum execution time of 1 second exceeded in %s on line %d
A::__get

Warning: Undefined property: A::$foo in %s on line %d
NULL
