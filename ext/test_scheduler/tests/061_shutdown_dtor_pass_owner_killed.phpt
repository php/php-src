--TEST--
test_scheduler: a fatal error in the shutdown iterator during the object-store pass skips the remaining destructors and does not resume the parked main
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
