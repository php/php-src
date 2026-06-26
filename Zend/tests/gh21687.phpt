--TEST--
GH-21687 (array_walk corrupts readonly and enum properties by wrapping them in IS_REFERENCE)
--FILE--
<?php
enum Foo: int {
    case Bar = 0;
}

$bar = Foo::Bar;
array_walk($bar, function($v) {});
var_dump($bar);

try {
    array_walk($bar, function(&$v) { $v = 1; });
} catch (\Error $e) {
    echo $e->getMessage() . "\n";
}
var_dump($bar);

class MyObj {
    public function __construct(public readonly string $name = "test") {}
}

$obj = new MyObj();
array_walk($obj, function($v) {});
echo $obj->name . "\n";

try {
    array_walk($obj, function(&$v) { $v = "modified"; });
} catch (\Error $e) {
    echo $e->getMessage() . "\n";
}
echo $obj->name . "\n";

class Container {
    public function __construct(public readonly array $items = ["a", "b"]) {}
}
$c = new Container();
array_walk_recursive($c, function($v) {});
var_dump($c->items);
?>
--EXPECT--
enum(Foo::Bar)
Cannot acquire reference to readonly property Foo::$name
enum(Foo::Bar)
test
Cannot acquire reference to readonly property MyObj::$name
test
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
}
