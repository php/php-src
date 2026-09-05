--TEST--
test_scheduler: cancel() — catchable at the suspend point, no re-park, first cancel wins
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

    // Cancelled means done waiting: parking again is refused on the spot.
    try {
        suspend();
        echo "unreachable suspend\n";
    } catch (TestScheduler\CancellationError $e) {
        echo "victim cannot suspend again\n";
    }

    return "cleanup done";
});

spawn(function () use ($victim) {
    cancel($victim);
    cancel($victim); // the second one is a no-op
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
victim cannot suspend again
cleanup done
unstarted: The coroutine has been cancelled
self-cancel caught
==DONE==
