--TEST--
test_scheduler: await() misuse — awaiting itself, awaited exception stays claimable
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
use function TestScheduler\{spawn, await};

// A coroutine cannot await itself.
$self = null;
$self = spawn(function () use (&$self) { await($self); });
try {
    await($self);
} catch (Error $e) {
    echo "self: ", $e->getMessage(), "\n";
}

// An exception someone holds a handle to is not "unhandled": every await
// rethrows it, and nothing is reported at shutdown.
$failing = spawn(function () { throw new LogicException("kept"); });
try {
    await($failing);
} catch (LogicException $e) {
    echo "first: ", $e->getMessage(), "\n";
}
try {
    await($failing);
} catch (LogicException $e) {
    echo "second: ", $e->getMessage(), "\n";
}
echo "getException: ", $failing->getException()->getMessage(), "\n";
echo "==DONE==\n";
?>
--EXPECT--
self: Cannot await a coroutine from within itself
first: kept
second: kept
getException: kept
==DONE==
