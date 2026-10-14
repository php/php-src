--TEST--
test_scheduler: await() returns results, rethrows exceptions, works from generators
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
use function TestScheduler\{spawn, suspend, resume, await, current};

$main = current();

$a = spawn(fn () => 42);
$b = spawn(function () { throw new RuntimeException("boom"); });

function gen($a): Generator {
    yield await($a); // awaiting inside a generator body
    yield 7;
}

spawn(function () use ($main, $a, $b) {
    echo "await a: ", await($a), "\n";
    try {
        await($b);
    } catch (RuntimeException $e) {
        echo "await b threw: ", $e->getMessage(), "\n";
    }
    echo "late await a: ", await($a), "\n"; // already finished
    foreach (gen($a) as $v) {
        echo "gen: $v\n";
    }
    resume($main);
});

suspend();
echo "main done\n";
?>
--EXPECT--
await a: 42
await b threw: boom
late await a: 42
gen: 42
gen: 7
main done
