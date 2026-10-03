--TEST--
Fiber keeps the object a class-string callable resolved to: $this of the creating method outlives it
--FILE--
<?php
class Target {
    public string $label = "kept";

    public function method(): void {
        echo "method: {$this->label}\n";
    }

    public function makeFiber(): Fiber {
        return new Fiber([Target::class, 'method']);
    }

    public function __destruct() {
        echo "released\n";
    }
}

// The only reference to the Target is the creating call's $this.
$fiber = (new Target)->makeFiber();
$filler = array_fill(0, 100, new stdClass());
$fiber->start();
echo "started\n";
unset($fiber);
echo "done\n";
?>
--EXPECT--
method: kept
released
started
done
