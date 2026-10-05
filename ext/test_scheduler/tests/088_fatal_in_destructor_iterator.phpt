--TEST--
test_scheduler: a fatal error in a destructor run by the shutdown iterator coroutine ends the queue and the parked main
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
class Fails {
    public function __destruct() {
        TestScheduler\spawn(function () {
            echo "coroutine ran\n";
        });
        eval('function redeclared() {}');
        eval('function redeclared() {}');
    }
}

class Waits {
    public function __destruct() {
        echo "waits: suspends\n";
        TestScheduler\await(TestScheduler\spawn(function () {
            TestScheduler\suspend();
        }));
        echo "waits: resumed\n";
    }
}

$fails = new Fails();
$waits = new Waits();
echo "main\n";
?>
--EXPECTF--
main
waits: suspends

Fatal error: Cannot redeclare function redeclared() (previously declared in %s : eval()'d code:1) in %s : eval()'d code on line 1
