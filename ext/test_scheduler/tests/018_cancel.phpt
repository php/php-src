--TEST--
test_scheduler: cancel() — catchable at the suspend point, first pending cancel wins, a later one is delivered again
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
use function TestScheduler\{spawn, await, suspend, cancel, current};

// A parked coroutine is cancelled: the CancellationError lands at its
// suspend point and is catchable there — cleanup runs, the body finishes.
$victim = spawn(function () {
    try {
        suspend();
        echo "unreachable\n";
    } catch (TestScheduler\CancellationError $e) {
        echo "victim caught: ", $e->getMessage(), "\n";
    }

    // Cancellation is a request: once delivered, the coroutine may park again
    // and the next cancel reaches it there.
    try {
        suspend();
        echo "unreachable suspend\n";
    } catch (TestScheduler\CancellationError $e) {
        echo "victim caught again\n";
    }

    return "cleanup done";
});

spawn(function () use ($victim) {
    cancel($victim);
    cancel($victim); // still pending: the second one is dropped

    // Runs after the victim has caught the first one and parked again.
    spawn(function () use ($victim) {
        echo "second cancel\n";
        cancel($victim);
    });
});

echo "await victim: ", await($victim), "\n";

// A queued coroutine cancelled before it ever ran: the body stays unrun and
// the cancellation is its outcome.
$unstarted = spawn(function () {
    echo "unreachable body\n";
});
cancel($unstarted);
try {
    await($unstarted);
} catch (TestScheduler\CancellationError $e) {
    echo "unstarted: ", $e->getMessage(), "\n";
}

// Self-cancel throws in place.
$self = spawn(function () {
    try {
        cancel(current());
        echo "unreachable\n";
    } catch (TestScheduler\CancellationError $e) {
        echo "self-cancel caught\n";
    }
});
await($self);

echo "==DONE==\n";
?>
--EXPECT--
await victim: victim caught: The coroutine has been cancelled
second cancel
victim caught again
cleanup done
unstarted: The coroutine has been cancelled
self-cancel caught
==DONE==
