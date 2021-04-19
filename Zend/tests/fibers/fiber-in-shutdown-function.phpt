--TEST--
Fiber in shutdown function
--FILE--
<?php

register_shutdown_function(function (): void {
    $fiber = new Fiber(function (): int {
        Fiber::suspend(1);
        Fiber::suspend(2);
        return 3;
    });

    var_dump($fiber->start());
    var_dump($fiber->resume());
    var_dump($fiber->resume());
    var_dump($fiber->getReturn());
});

?>
--EXPECT--
int(1)
int(2)
NULL
int(3)
