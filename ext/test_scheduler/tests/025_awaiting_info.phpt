--TEST--
getAwaitingInfo(): await registers a description, enqueue wipes the whole vector
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php

use function TestScheduler\{spawn, suspend, resume, await, current};

$main = current();

$target = spawn(function () {
    suspend();
    return 'done';
});

/* Not waiting yet: never started. */
var_dump($target->getAwaitingInfo());

$waiter = spawn(function () use ($target, $main) {
    $result = await($target);
    resume($main);
    return $result;
});

spawn(function () use ($main, $target, $waiter) {
    echo "-- waiter parked in await --\n";
    var_dump($waiter->isSuspended());
    var_dump($waiter->getAwaitingInfo());

    /* A plain suspend() says nothing about itself. */
    var_dump($target->getAwaitingInfo());

    /* A stray resume() puts the waiter back into the queue: the enqueue
     * wipes every description at once — the full cleanup. */
    resume($waiter);
    echo "-- queued by a stray resume: info wiped --\n";
    var_dump($waiter->getAwaitingInfo());

    resume($main);
});

suspend();

/* The woken waiter re-parked in await and registered the wait again. */
echo "-- waiter re-parked --\n";
var_dump($waiter->getAwaitingInfo());

/* The target finishes, the waiter consumes the result: nothing waits now. */
resume($target);
suspend();

echo "-- both finished --\n";
var_dump($waiter->isFinished());
var_dump($waiter->getResult());
var_dump($waiter->getAwaitingInfo());

?>
--EXPECTF--
array(0) {
}
-- waiter parked in await --
bool(true)
array(1) {
  [0]=>
  string(%d) "await: coroutine #%d"
}
array(0) {
}
-- queued by a stray resume: info wiped --
array(0) {
}
-- waiter re-parked --
array(1) {
  [0]=>
  string(%d) "await: coroutine #%d"
}
-- both finished --
bool(true)
string(4) "done"
array(0) {
}
