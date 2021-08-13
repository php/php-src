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

Fatal error: Fatal error in fiber in %sfatal-error-with-multiple-fibers.php on line %d
