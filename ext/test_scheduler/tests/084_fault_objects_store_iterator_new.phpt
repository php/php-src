--TEST--
test_scheduler: the object store's destructors go on in the driving coroutine when no iterator can be created
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
        TestScheduler\await(TestScheduler\spawn(fn() => null));
        echo "{$this->name} end\n";
    }
}

/* Cycles outlive the symbol table: the object store's pass destroys them. */
foreach (['x', 'y'] as $name) {
    $waiter = new Waiter($name);
    $waiter->self = $waiter;
}
unset($waiter);

ini_set('test_scheduler.fail_new_coroutine', 1);
echo "end of script\n";
?>
--EXPECT--
end of script
x start
x end
y start
y end
