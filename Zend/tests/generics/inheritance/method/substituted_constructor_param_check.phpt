--TEST--
Inherited constructors: substituted parameter type is enforced at runtime on the substituted child
--FILE--
<?php
class Box<T> {
    public function __construct(T $v) {
        var_dump($v);
    }
}

class IntBox extends Box<int> {}

new Box(42);
new Box("hello");
new Box([1, 2]);

try {
    new IntBox("hello");
} catch (TypeError $e) {
    echo "caught: ", $e->getMessage(), "\n";
}

new IntBox(99);

class PromBox<T> {
    public function __construct(public T $v) {}
}
class IntPromBox extends PromBox<int> {}

try {
    new IntPromBox("hello");
} catch (TypeError $e) {
    echo "prom: ", $e->getMessage(), "\n";
}
?>
--EXPECTF--
int(42)
string(5) "hello"
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
caught: Box::__construct(): Argument #1 ($v) must be of type int, string given, called in %s on line %d
int(99)
prom: PromBox::__construct(): Argument #1 ($v) must be of type int, string given, called in %s on line %d
