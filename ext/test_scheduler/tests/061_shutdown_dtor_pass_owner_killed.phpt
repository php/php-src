--TEST--
Pass-owning shutdown iterator dies mid-pass: warn and give up on the remaining destructors
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php

class Holder {
    public static array $keep = [];
}

class ParksForever {
    public function __destruct() {
        echo "ParksForever::dtor enter\n";
        TestScheduler\await(TestScheduler\spawn(function () {
            TestScheduler\suspend();
        }));
        echo "ParksForever::dtor leave\n";
    }
}

class Fatal {
    public function __destruct() {
        echo "Fatal::dtor enter\n";
        trigger_error("boom", E_USER_ERROR);
    }
}

class Skipped {
    public function __destruct() {
        echo "Skipped::dtor (must not run)\n";
    }
}

$a = new ParksForever();   // symbol table, rc=1: dies in the symbol pass
Holder::$keep[] = new Fatal();
Holder::$keep[] = new Skipped();

echo "==DONE==\n";
?>
--EXPECTF--
==DONE==
ParksForever::dtor enter
Fatal::dtor enter

Deprecated: Passing E_USER_ERROR to trigger_error() is deprecated since 8.4, throw an exception or call exit with a string message instead in %s on line %d

Fatal error: boom in %s on line %d

Warning: Shutdown destructors coroutine was not finished properly in Unknown on line %d

Fatal error: Uncaught TestScheduler\CancellationError: Deadlock detected in [no active file]:%d
Stack trace:
#0 {main}
  thrown in [no active file] on line %d

Fatal error: Uncaught TestScheduler\DeadlockError: Deadlock detected: no active coroutines, 2 coroutines in waiting in [no active file]:%d
Stack trace:
#0 {main}
  thrown in [no active file] on line %d
