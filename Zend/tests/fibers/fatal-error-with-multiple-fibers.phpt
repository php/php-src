--TEST--
Fatal error in a fiber with other active fibers
--FILE--
<?php

$fiber1 = new Fiber(function (): void {
    try {
        \Fiber::suspend(1);
    } finally {
        echo "not executed";
    }
});

$fiber2 = new Fiber(function (): void {
    \Fiber::suspend(2);
    trigger_error("Fatal error in fiber", E_USER_ERROR);
});

var_dump($fiber1->start());
var_dump($fiber2->start());
$fiber2->resume();

?>
--EXPECTF--
int(1)
int(2)

Deprecated: Passing E_USER_ERROR to trigger_error() is deprecated since 8.4, throw an exception or call exit with a string message instead in %s on line %d

Fatal error: Fatal error in fiber in %sfatal-error-with-multiple-fibers.php on line %d
