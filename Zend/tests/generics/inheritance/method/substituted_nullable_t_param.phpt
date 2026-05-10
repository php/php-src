--TEST--
Inherited methods: nullable T parameter accepts null and the substituted concrete type, rejects others
--FILE--
<?php
class Box<T> {
    public function take(?T $v): void {
        var_dump($v);
    }
}

class IntBox extends Box<int> {}

$b = new IntBox();

$b->take(null);
$b->take(42);

try {
    $b->take("hello");
} catch (TypeError $e) {
    echo "wrong: ", $e->getMessage(), "\n";
}
?>
--EXPECTF--
NULL
int(42)
wrong: Box::take(): Argument #1 ($v) must be of type ?int, string given, called in %s on line %d
