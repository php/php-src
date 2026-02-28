--TEST--
Generic type argument forwarding (new Box<T> inside generic methods)
--FILE--
<?php
declare(strict_types=1);

class Box<T> {
    public T $value;
    public function __construct(T $value) { $this->value = $value; }
    public function get(): T { return $this->value; }
}

// Static method forwarding type args
class Factory<T> {
    public static function create(T $val): Box<T> {
        return new Box<T>($val);
    }
}

$box = Factory<int>::create(42);
echo $box->get() . "\n";

$sbox = Factory<string>::create("hello");
echo $sbox->get() . "\n";

// Instance method forwarding type args
class Wrapper<T> {
    public function wrap(T $val): Box<T> {
        return new Box<T>($val);
    }
}

$w = new Wrapper<int>();
$b = $w->wrap(99);
echo $b->get() . "\n";

// Type enforcement on forwarded object
try {
    $box->value = "wrong";
} catch (TypeError $e) {
    echo "TypeError caught\n";
}

// Nested forwarding: Factory creates Wrapper which creates Box
class NestedFactory<T> {
    public static function makeWrapped(T $val): Box<T> {
        $w = new Wrapper<T>();
        return $w->wrap($val);
    }
}

$nb = NestedFactory<int>::makeWrapped(77);
echo $nb->get() . "\n";

echo "OK\n";
?>
--EXPECT--
42
hello
99
TypeError caught
77
OK
