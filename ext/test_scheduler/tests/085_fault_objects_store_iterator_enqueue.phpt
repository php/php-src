--TEST--
test_scheduler: the object store's destructors go on in the driving coroutine when their iterator cannot be queued
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
class Waiter {
    public $self;

    public function __construct(public string $name) {}

    public function __destruct() {
        echo "{$this->name} start\n";
        TestScheduler\await(TestScheduler\spawn(function () {
            echo "{$this->name} spawned\n";
        }));
        echo "{$this->name} end\n";
    }
}

/* Cycles outlive the symbol table: the object store's pass destroys them. */
foreach (['x', 'y'] as $name) {
    $waiter = new Waiter($name);
    $waiter->self = $waiter;
}
unset($waiter);

/* The spawn is the first queued coroutine, the iterator the second. */
ini_set('test_scheduler.fail_enqueue', 2);
echo "end of script\n";
?>
--EXPECT--
end of script
x start
x spawned
x end
y start
y spawned
y end
