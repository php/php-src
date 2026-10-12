--TEST--
test_scheduler: a destructor iterator the hand-off cannot queue leaves the waiting iterator to finish the destructors
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
use function TestScheduler\{spawn, await};

class Waiter {
    public $self;

    public function __construct(public string $name) {
        $this->self = $this;
    }

    public function __destruct() {
        echo "{$this->name} start\n";

        if ($this->name === "a") {
            // The spawn below is the first coroutine queued, the hand-off's iterator the second.
            ini_set('test_scheduler.fail_enqueue', 2);
            await(spawn(function () {
                echo "spawned\n";
            }));
        }

        echo "{$this->name} end\n";
    }
}

new Waiter('a');
new Waiter('b');
var_dump(gc_collect_cycles());
?>
--EXPECT--
a start
spawned
a end
b start
b end
int(2)
