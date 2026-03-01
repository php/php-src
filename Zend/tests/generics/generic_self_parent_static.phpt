--TEST--
Generic class: self, parent, and static in generic context
--FILE--
<?php
declare(strict_types=1);

class Box<T> {
    protected T $value;

    public function __construct(T $value) {
        $this->value = $value;
    }

    public function get(): T {
        return $this->value;
    }

    // self return type in generic class
    public function withValue(T $value): self {
        $new = clone $this;
        $new->value = $value;
        return $new;
    }

    // static return type in generic class
    public static function create(mixed $value): static {
        return new static($value);
    }
}

class IntBox extends Box<int> {
    // parent reference in child of generic class
    public function getParentClass(): string {
        return parent::class;
    }
}

// 1. self return type
$box = new Box<int>(42);
$box2 = $box->withValue(99);
echo "1. original: " . $box->get() . "\n";
echo "1. new: " . $box2->get() . "\n";

// 2. static return type (late static binding)
$ib = IntBox::create(7);
echo "2. class: " . get_class($ib) . "\n";
echo "2. value: " . $ib->get() . "\n";

// 3. parent reference
$ib2 = new IntBox(10);
echo "3. parent: " . $ib2->getParentClass() . "\n";

// 4. self in type enforcement context
$box3 = new Box<string>("hello");
$box4 = $box3->withValue("world");
echo "4. " . $box4->get() . "\n";

// Type enforcement still works through self
try {
    $box3->withValue([1, 2]); // T=string, passing array
} catch (TypeError $e) {
    echo "4. TypeError OK\n";
}

echo "Done.\n";
?>
--EXPECT--
1. original: 42
1. new: 99
2. class: IntBox
2. value: 7
3. parent: Box
4. world
4. TypeError OK
Done.
