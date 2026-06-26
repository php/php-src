--TEST--
Inherited methods: both substituted parameter and return type are enforced; entry fires before return
--FILE--
<?php
class Box<T> {
    public function passthrough(T $v): T { return $v; }
}

class IntBox extends Box<int> {}

$b = new IntBox();

// Wrong type at entry: fails on parameter check, body never runs.
try {
    $b->passthrough("hello");
} catch (TypeError $e) {
    echo "entry: ", $e->getMessage(), "\n";
}

// Correct type passes through both checks.
var_dump($b->passthrough(42));

// Parent's mixed bound accepts and returns anything.
$p = new Box();
var_dump($p->passthrough("hello"));
var_dump($p->passthrough(42));
?>
--EXPECTF--
entry: Box::passthrough(): Argument #1 ($v) must be of type int, string given, called in %s on line %d
int(42)
string(5) "hello"
int(42)
