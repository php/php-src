--TEST--
test_scheduler: the shutdown destructors go on in the driving coroutine when no iterator can be created
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
        TestScheduler\await(TestScheduler\spawn(fn() => null));
        echo "{$this->name} end\n";
    }
}

$a = new Waiter('a');
$b = new Waiter('b');

/* Each destructor's await would start an iterator to carry the pass on. */
ini_set('test_scheduler.fail_new_coroutine', 1);
echo "end of script\n";
?>
--EXPECT--
end of script
b start
b end
a start
a end
