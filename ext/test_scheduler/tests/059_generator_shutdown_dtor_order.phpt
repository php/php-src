--TEST--
GH-15866: Core dumped in Zend/zend_generators.c — under test_scheduler
--DESCRIPTION--
The expected output departs from the upstream
Zend/tests/generators/gh15866.phpt. Under a scheduler a fiber parked at
Fiber::suspend() is not collected by gc_collect_cycles() when the cycle runs
through its body's closure: the closure sits in the coroutine's fcall, and a
parked fiber keeps its coroutine out of the collector's view, as the scheduler
holds references to it the collector cannot count. The fiber goes when the
scheduler ends it at shutdown, so the generator's and the fiber's finally
blocks and the destructor follow "==DONE==". The scheduler RFC lists this
behavior change as "A fiber parked in Fiber::suspend() is ended by the
scheduler, not collected".
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php

class Canary {
    public function __construct(public mixed $value) {}
    public function __destruct() {
        printf("%s\n", __METHOD__);
    }
}

function g() {
    Fiber::suspend();
}

function f($canary) {
    try {
        var_dump(yield from g());
    } finally {
        print "Generator finally\n";
    }
}

$canary = new Canary(null);
$iterable = f($canary);
$fiber = new Fiber(function () use ($iterable, $canary) {
    try {
        $iterable->next();
    } finally {
        print "Fiber finally\n";
    }
});
$canary->value = $fiber;
$fiber->start();

// Reset roots
gc_collect_cycles();

// Add to roots, create garbage cycles
$fiber = $iterable = $canary = null;

print "Collect cycles\n";
gc_collect_cycles();

?>
==DONE==
--EXPECT--
Collect cycles
==DONE==
Generator finally
Fiber finally
Canary::__destruct
