--TEST--
Generic class: generic objects work across fiber suspend/resume
--FILE--
<?php

class Box<T> {
    public function __construct(public T $value) {}
    public function get(): T { return $this->value; }
}

$fiber = new Fiber(function () {
    $b = new Box<int>(42);
    echo "Fiber: " . $b->get() . "\n";

    // Suspend and pass a generic object out
    Fiber::suspend($b);

    // After resume, modify and check type enforcement
    $b->value = 100;
    echo "Fiber resumed: " . $b->get() . "\n";

    try {
        $b->value = "not an int";
    } catch (TypeError $e) {
        echo "Fiber TypeError: type enforced\n";
    }
});

// Start the fiber and receive the generic object
$box = $fiber->start();
echo "Main received: ";
var_dump($box);

// Resume the fiber
$fiber->resume();

echo "OK\n";
?>
--EXPECTF--
Fiber: 42
Main received: object(Box<int>)#%d (1) {
  ["value"]=>
  int(42)
}
Fiber resumed: 100
Fiber TypeError: type enforced
OK
