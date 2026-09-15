--TEST--
test_scheduler: coroutine lifetime edges observed via WeakReference
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
use function TestScheduler\{spawn, await, suspend, resume, cancel};

class Box { public $co; public function __construct($co) { $this->co = $co; } }
class Carrier extends RuntimeException { public $co; }

echo "-- 1: cycle via own closure, handle dropped while parked --\n";
$co1 = null;
$co1 = spawn(function () use (&$co1) { $self = $co1; suspend(); });
$w1 = WeakReference::create($co1);
$h1 = $co1;
unset($co1);
gc_collect_cycles(); // runs while co1 is parked: the scheduler anchors it
echo "parked: ", ($w1->get() === null ? "collected" : "alive"), "\n";
resume($h1);
await($h1);
unset($h1);
gc_collect_cycles(); // co -> closure -> co collapses once finished
echo "finished: ", ($w1->get() === null ? "collected" : "alive"), "\n";

echo "-- 2: cycle via result --\n";
$co2 = null;
$co2 = spawn(function () use (&$co2) { return new Box($co2); });
$w2 = WeakReference::create($co2);
await($co2);
unset($co2);
gc_collect_cycles();
echo "finished: ", ($w2->get() === null ? "collected" : "alive"), "\n";

echo "-- 3: cycle via stored exception --\n";
$co3 = null;
$co3 = spawn(function () use (&$co3) {
    $e = new Carrier("boom");
    $e->co = $co3;
    throw $e;
});
$w3 = WeakReference::create($co3);
try { await($co3); } catch (Carrier $e) { echo "caught: ", $e->getMessage(), "\n"; unset($e); }
unset($co3);
gc_collect_cycles();
echo "finished: ", ($w3->get() === null ? "collected" : "alive"), "\n";

echo "-- 4: an awaiter keeps the coroutine alive --\n";
$co4 = null;
$co4 = spawn(function () use (&$co4) { $self = $co4; suspend(); return "ok"; });
$w4 = WeakReference::create($co4);
$checker = spawn(function () use ($co4) {
    echo "await returns: ", await($co4), "\n";
});
$h4 = $co4;
unset($co4);
gc_collect_cycles();
echo "awaited: ", ($w4->get() === null ? "collected" : "alive"), "\n";
resume($h4); unset($h4);
await($checker); unset($checker);
gc_collect_cycles();
echo "finished: ", ($w4->get() === null ? "collected" : "alive"), "\n";

echo "-- 5: cancelled coroutine with a closure cycle --\n";
$co5 = null;
$co5 = spawn(function () use (&$co5) {
    $self = $co5;
    try { suspend(); } catch (TestScheduler\CancellationError $e) { echo "cancelled\n"; }
});
$w5 = WeakReference::create($co5);
spawn(function () use ($co5) { cancel($co5); });
$h5 = $co5; unset($co5);
try { await($h5); } catch (TestScheduler\CancellationError $e) { echo "await threw\n"; }
unset($h5);
gc_collect_cycles();
echo "finished: ", ($w5->get() === null ? "collected" : "alive"), "\n";
?>
--EXPECT--
-- 1: cycle via own closure, handle dropped while parked --
parked: alive
finished: collected
-- 2: cycle via result --
finished: collected
-- 3: cycle via stored exception --
caught: boom
finished: collected
-- 4: an awaiter keeps the coroutine alive --
await returns: awaited: alive
ok
finished: collected
-- 5: cancelled coroutine with a closure cycle --
cancelled
finished: collected
