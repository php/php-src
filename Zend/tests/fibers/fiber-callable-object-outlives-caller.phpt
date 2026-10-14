--TEST--
Fiber holds the object a callable resolved to in the creating method ($this for [A::class, 'm'] or "A::m")
--FILE--
<?php
class A {
    public ?Fiber $fiber = null;

    public function __construct(public string $name) {}

    public function m() {
        echo "m on {$this->name}\n";
    }

    public function waits() {
        Fiber::suspend();
        echo "resumed on {$this->name}\n";
    }

    public function make(string $method) {
        return new Fiber([A::class, $method]);
    }

    public function makeFromString() {
        return new Fiber("A::m");
    }

    public function keep() {
        $this->fiber = new Fiber([A::class, 'm']);
    }

    public function __destruct() {
        echo "released {$this->name}\n";
    }
}

echo "-- started after its creator's last reference went\n";
$fiber = (new A("a1"))->make('m');
$fiber->start();
echo "after start\n";

echo "-- a string callable\n";
$fiber = (new A("a2"))->makeFromString();
$fiber->start();
echo "after start\n";

echo "-- never started, released with the fiber\n";
$fiber = (new A("a3"))->make('m');
unset($fiber);
echo "after unset\n";

echo "-- suspended while its object has no other reference\n";
$a = new A("a4");
$fiber = $a->make('waits');
$fiber->start();
unset($a);
$fiber->resume();
echo "after resume\n";

echo "-- a cycle through the fiber is collected\n";
$a = new A("a5");
$a->keep();
unset($a);
var_dump(gc_collect_cycles());
echo "end\n";
?>
--EXPECT--
-- started after its creator's last reference went
m on a1
released a1
after start
-- a string callable
m on a2
released a2
after start
-- never started, released with the fiber
released a3
after unset
-- suspended while its object has no other reference
resumed on a4
released a4
after resume
-- a cycle through the fiber is collected
released a5
int(2)
end
