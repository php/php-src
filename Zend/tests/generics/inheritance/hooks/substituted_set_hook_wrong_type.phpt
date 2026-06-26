--TEST--
Property hooks: set hook with T parameter rejects wrong-type writes on the substituted view
--FILE--
<?php
class Box<T> {
    private T $backing;
    public T $value {
        get => $this->backing;
        set { $this->backing = $value; }
    }
    public function __construct(T $v) { $this->backing = $v; }
}

class IntBox extends Box<int> {}

$b = new IntBox(0);
$b->value = 42;
var_dump($b->value);

try {
    $b->value = "hello";
} catch (TypeError $e) {
    echo "caught: ", $e->getMessage(), "\n";
}

$p = new Box(1);
$p->value = "anything";
$p->value = [1, 2];
echo "parent ok\n";
?>
--EXPECTF--
int(42)
caught: Box::$value::set(): Argument #1 ($value) must be of type int, string given, called in %s on line %d
parent ok
