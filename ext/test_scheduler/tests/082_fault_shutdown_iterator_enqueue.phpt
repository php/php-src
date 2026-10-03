--TEST--
test_scheduler: the shutdown destructors go on in the driving coroutine when their iterator cannot be queued
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
class Waiter {
    public function __construct(public string $name) {}

    public function __destruct() {
        echo "{$this->name} start\n";
        TestScheduler\await(TestScheduler\spawn(function () {
            echo "{$this->name} spawned\n";
        }));
        echo "{$this->name} end\n";
    }
}

$a = new Waiter('a');

/* The spawn is the first queued coroutine, the iterator the second. */
ini_set('test_scheduler.fail_enqueue', 2);
echo "end of script\n";
?>
--EXPECT--
end of script
a start
a spawned
a end
