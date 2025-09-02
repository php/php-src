--TEST--
GH-15330 002: Do not scan generator frames more than once
--FILE--
<?php

function g() {
    yield 'foo';
    Fiber::suspend();
}

function f() {
    var_dump(yield from g());
}

$iterable = f();

$fiber = new Fiber(function () use ($iterable) {
    var_dump($iterable->current());
    $iterable->next();
    var_dump("not executed");
});

$ref = $fiber;

$fiber->start();

gc_collect_cycles();

?>
==DONE==
--EXPECT--
string(3) "foo"
==DONE==
