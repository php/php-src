--TEST--
test_scheduler: a coroutine records its result, and an exception nobody awaits is fatal
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
use function TestScheduler\{spawn, suspend, resume, current};

$main = current();

$ok = spawn(fn() => 42);
spawn(function () use ($main, $ok) {
    resume($main);
});

suspend();

var_dump($ok->isFinished(), $ok->getResult());

/* An exception that reaches the top of a coroutine ends the request, exactly
 * as it does at the top of the script. */
spawn(function () { throw new RuntimeException("boom"); });
?>
--EXPECTF--
bool(true)
int(42)

Fatal error: Uncaught RuntimeException: boom in %s:%d
Stack trace:
#0 %s
#1 {main}
  thrown in %s on line %d
