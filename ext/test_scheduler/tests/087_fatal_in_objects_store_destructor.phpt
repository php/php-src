--TEST--
test_scheduler: a fatal error in a destructor of the object store's pass ends the coroutines queued before it
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
class Fails {
    public $self;

    public function __destruct() {
        TestScheduler\spawn(function () {
            echo "coroutine ran\n";
        });
        eval('function redeclared() {}');
        eval('function redeclared() {}');
    }
}

/* A cycle outlives the symbol table: the object store's pass destroys it. */
$fails = new Fails();
$fails->self = $fails;
unset($fails);
echo "main\n";
?>
--EXPECTF--
main

Fatal error: Cannot redeclare function redeclared() (previously declared in %s : eval()'d code:1) in %s : eval()'d code on line 1
