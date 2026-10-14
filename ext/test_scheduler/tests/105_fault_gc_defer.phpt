--TEST--
test_scheduler: destructors whose hand-off microtask cannot be deferred run one after another, and the refusal is thrown nowhere
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
            await(spawn(function () {
                echo "spawned\n";
            }));
        }

        echo "{$this->name} end\n";
    }
}

new Waiter('a');
new Waiter('b');

// The first iterator's hand-off is the first microtask deferred.
ini_set('test_scheduler.fail_defer', 1);
var_dump(gc_collect_cycles());
?>
--EXPECT--
a start
spawned
a end
b start
b end
int(2)
