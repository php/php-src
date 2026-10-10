--TEST--
GH-15330 003: Do not scan generator frames more than once — under test_scheduler
--DESCRIPTION--
The expected output departs from the upstream
Zend/tests/generators/gh15330-003.phpt. Under a scheduler a fiber parked at
Fiber::suspend() is not collected by gc_collect_cycles() when the cycle runs
through its body's closure: the closure sits in the coroutine's fcall, and a
parked fiber keeps its coroutine out of the collector's view, as the scheduler
holds references to it the collector cannot count. The fiber goes when the
scheduler ends it at shutdown, so the destructor's output follows "==DONE==".
The scheduler RFC lists this behavior change as "A fiber parked in
Fiber::suspend() is ended by the scheduler, not collected".
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php

class It implements \IteratorAggregate
{
    public function getIterator(): \Generator
    {
        yield 'foo';
        Fiber::suspend();
        var_dump("not executed");
    }
}

class Canary {
    public function __construct(public mixed $value) {}
    public function __destruct() {
        var_dump(__METHOD__);
    }
}

function f($canary) {
    var_dump(yield from new It());
}

$canary = new Canary(null);

$iterable = f($canary);

$fiber = new Fiber(function () use ($iterable, $canary) {
    var_dump($canary, $iterable->current());
    $iterable->next();
    var_dump("not executed");
});

$canary->value = $fiber;

$fiber->start();

$iterable->current();

$fiber = $iterable = $canary = null;

gc_collect_cycles();

?>
==DONE==
--EXPECTF--
object(Canary)#%d (1) {
  ["value"]=>
  object(Fiber)#%d (0) {
  }
}
string(3) "foo"
==DONE==
string(18) "Canary::__destruct"
