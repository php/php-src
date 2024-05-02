--TEST--
Bug #79657: "yield from" hangs when invalid value encountered
--FILE--
<?php

function throwException(): iterable
{
    throw new Exception();
}

function loop(): iterable
{
    $callbacks = [
        function () {
            yield 'first';
        },
        function () {
            yield from throwException();
        }
    ];

    foreach ($callbacks as $callback) {
        yield from $callback();
    }
}

function get(string $first, int $second): array
{
    return [];
}

get(...loop());

?>
--EXPECTF--
Fatal error: Uncaught Exception in %s:%d
Stack trace:
#0 %s(%d): throwException()
#1 %s(%d): {closure:%s:%d}()
#2 %s(%d): loop()
#3 {main}
  thrown in %s on line %d
