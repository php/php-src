--TEST--
test_scheduler: a yield-from chain parked in a coroutine unwinds cleanly at shutdown
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
use function TestScheduler\{spawn, suspend};

function inner() {
    try {
        echo "inner enter\n";
        suspend(); // park the coroutine deep inside the chain
        yield 1;
        echo "not executed (inner)\n";
    } finally {
        echo "inner finally\n";
    }
}

function outer() {
    try {
        yield from inner();
    } finally {
        echo "outer finally\n";
    }
}

$g = outer();
spawn(function () use ($g) {
    try {
        $g->current();
        echo "not executed (coro)\n";
    } finally {
        echo "coro finally\n";
    }
});
echo "==DONE==\n";
?>
--EXPECT--
==DONE==
inner enter
inner finally
outer finally
coro finally
