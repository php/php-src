--TEST--
Property hooks: asymmetric-visibility hook keeps substituted enforcement on both reads and writes
--FILE--
<?php
class Box<T> {
    public private(set) T $value {
        get => $this->value;
        set { $this->value = $value; }
    }
    public function __construct(T $v) { $this->value = $v; }
    public function rebind(T $v): void { $this->value = $v; }
}

class IntBox extends Box<int> {}

$b = new IntBox(42);
var_dump($b->value);

$b->rebind(7);
var_dump($b->value);

try {
    $b->rebind("nope");
} catch (TypeError $e) {
    echo "caught: ", $e->getMessage(), "\n";
}
?>
--EXPECTF--
int(42)
int(7)
caught: Box::rebind(): Argument #1 ($v) must be of type int, string given, called in %s on line %d
