--TEST--
Generic class: child inherits parent's generic constructor
--FILE--
<?php
declare(strict_types=1);

class Box<T> {
    private T $value;
    public function __construct(T $value) { $this->value = $value; }
    public function get(): T { return $this->value; }
}

// Child with no constructor — inherits parent's
class LabeledBox<T> extends Box<T> {
    public function label(): string { return "LabeledBox"; }
}

// Child with bound type and no constructor
class IntBox extends Box<int> {}

// Child with own constructor calling parent
class NamedBox<T> extends Box<T> {
    private string $name;
    public function __construct(string $name, T $value) {
        $this->name = $name;
        parent::__construct($value);
    }
    public function getName(): string { return $this->name; }
}

// 1. Inherited constructor with explicit type args
$lb = new LabeledBox<string>("hello");
echo "1. " . $lb->get() . " - " . $lb->label() . "\n";

// 2. Inherited constructor — type enforcement
try {
    $lb2 = new LabeledBox<int>("not int");
} catch (TypeError $e) {
    echo "2. TypeError OK\n";
}

// 3. Bound type with inherited constructor
$ib = new IntBox(42);
echo "3. " . $ib->get() . "\n";

try {
    $ib2 = new IntBox("not int");
} catch (TypeError $e) {
    echo "3. TypeError OK\n";
}

// 4. Own constructor calling parent
$nb = new NamedBox<float>("temp", 98.6);
echo "4. " . $nb->getName() . ": " . $nb->get() . "\n";

// 5. Inferred type through explicit type args on child
$lb3 = new LabeledBox<int>(42);
echo "5. " . $lb3->get() . " - " . $lb3->label() . "\n";

try {
    $lb3->get(); // returns int, fine
    echo "5. class: " . get_class($lb3) . "\n";
} catch (TypeError $e) {
    echo "FAIL: " . $e->getMessage() . "\n";
}

echo "Done.\n";
?>
--EXPECT--
1. hello - LabeledBox
2. TypeError OK
3. 42
3. TypeError OK
4. temp: 98.6
5. 42 - LabeledBox
5. class: LabeledBox
Done.
