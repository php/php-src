--TEST--
test_scheduler: spawn() and a Fiber keep the object a class-string callable resolved to
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php
class Target {
    public function __construct(public string $label) {}

    public function method(): void {
        echo "method: {$this->label}\n";
    }

    public function spawnMethod() {
        return TestScheduler\spawn([Target::class, 'method']);
    }

    public function makeFiber(): Fiber {
        return new Fiber([Target::class, 'method']);
    }

    public function __destruct() {
        echo "released {$this->label}\n";
    }
}

// The only reference to each Target is the creating call's $this. The coroutine
// and the Fiber hold their callable until the request frees them.
$coroutine = (new Target("spawned"))->spawnMethod();
$fiber = (new Target("fiber"))->makeFiber();
$filler = array_fill(0, 100, new stdClass());
TestScheduler\await($coroutine);
$fiber->start();
echo "done\n";
?>
--EXPECT--
method: spawned
method: fiber
done
released fiber
released spawned
