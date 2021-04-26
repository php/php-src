--TEST--
Pause fiber in destruct
--FILE--
<?php

$fiber = new Fiber(function (): int {
    $object = new class() {
        public function __destruct()
        {
            Fiber::suspend(2);
        }
    };

    Fiber::suspend(1);

    unset($object);

    Fiber::suspend(3);

    return 4;
});

var_dump($fiber->start());
var_dump($fiber->resume());
var_dump($fiber->resume());
var_dump($fiber->resume());
var_dump($fiber->getReturn());

?>
--EXPECT--
int(1)
int(2)
int(3)
NULL
int(4)
