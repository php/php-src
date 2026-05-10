--TEST--
Property hooks: get hook delegating to $this->backing returns the substituted-typed value, no extra check needed
--FILE--
<?php
class Box<T> {
    public T $value {
        get => $this->value;
    }
    public function __construct(T $v) { $this->value = $v; }
}

class IntBox extends Box<int> {}

$b = new IntBox(42);
var_dump($b->value);

try {
    new IntBox("hello");
} catch (TypeError $e) {
    echo "caught: ", $e->getMessage(), "\n";
}
?>
--EXPECTF--
int(42)
caught: Box::__construct(): Argument #1 ($v) must be of type int, string given, called in %s on line %d
