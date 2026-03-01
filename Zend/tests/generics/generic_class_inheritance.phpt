--TEST--
Generic class: inheritance with bound type arguments
--FILE--
<?php
class Box<T> {
    private $value;
    public function __construct(T $value) { $this->value = $value; }
    public function get(): T { return $this->value; }
}

class IntBox extends Box<int> {}

$ib = new IntBox(99);
echo $ib->get() . "\n";

try {
    $bad = new IntBox("not an int");
} catch (TypeError $e) {
    echo "TypeError caught\n";
}
?>
--EXPECT--
99
TypeError caught
