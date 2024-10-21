--TEST--
GH-15866: Core dumped in Zend/zend_generators.c
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
Canary::__destruct
Generator finally
Fiber finally
==DONE==
