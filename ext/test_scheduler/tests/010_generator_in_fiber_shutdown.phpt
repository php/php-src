--TEST--
test_scheduler: a generator parked at Fiber::suspend() unwinds with its fiber at shutdown
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
function gen() {
    try {
        echo "gen start\n";
        Fiber::suspend();
        yield 1;
        echo "not executed (gen)\n";
    } finally {
        echo "gen finally\n";
    }
}

$g = gen();
$fiber = new Fiber(function () use ($g) {
    try {
        $g->current(); // enters gen body, parks at Fiber::suspend()
        echo "not executed (fiber)\n";
    } finally {
        echo "fiber finally\n";
    }
});
$fiber->start();
echo "==DONE==\n";
?>
--EXPECT--
gen start
==DONE==
gen finally
fiber finally
