--TEST--
Inherited methods: substituted parameter type is enforced at runtime even when the parent's bound is mixed
--FILE--
<?php
class Box<T> {
    public function take(T $v): void {
        var_dump($v);
    }
}

class IntBox extends Box<int> {}

$b = new IntBox();
try {
    $b->take("hello");
} catch (TypeError $e) {
    echo "caught: ", $e->getMessage(), "\n";
}

// Parent-side call with mixed bound still accepts anything.
$p = new Box();
$p->take("hello");
$p->take(42);
$p->take([1, 2]);
?>
--EXPECTF--
caught: Box::take(): Argument #1 ($v) must be of type int, string given, called in %s on line %d
string(5) "hello"
int(42)
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
