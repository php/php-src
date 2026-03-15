--TEST--
Generic class: serialize and unserialize preserves generic type args
--FILE--
<?php
declare(strict_types=1);

class Box<T> {
    public function __construct(public T $value) {}
}

// Single type arg
$b = new Box<int>(42);
$s = serialize($b);
echo "Serialized: " . $s . "\n";

$b2 = unserialize($s);
echo "Class: " . get_class($b2) . "\n";
var_dump($b2);

// Type enforcement after unserialize
try {
    $b2->value = "not an int";
} catch (TypeError $e) {
    echo "TypeError: type enforced\n";
}

// Multiple type args
class Pair<A, B> {
    public function __construct(public A $first, public B $second) {}
}

$p = new Pair<string, int>("hello", 42);
$ps = serialize($p);
$p2 = unserialize($ps);
var_dump($p2);

// With __serialize/__unserialize
class CustomBox<T> {
    public function __construct(public T $value) {}
    public function __serialize(): array { return ["v" => $this->value]; }
    public function __unserialize(array $data): void { $this->value = $data["v"]; }
}

$cb = new CustomBox<string>("world");
$cbs = serialize($cb);
$cb2 = unserialize($cbs);
var_dump($cb2);

// Non-generic class is unaffected
class Plain {
    public function __construct(public int $x) {}
}
$pl = new Plain(99);
$pls = serialize($pl);
echo "Plain: " . $pls . "\n";
$pl2 = unserialize($pls);
var_dump($pl2);

echo "OK\n";
?>
--EXPECTF--
Serialized: O:8:"Box<int>":1:{s:5:"value";i:42;}
Class: Box
object(Box<int>)#2 (1) {
  ["value"]=>
  int(42)
}
TypeError: type enforced
object(Pair<string, int>)#%d (2) {
  ["first"]=>
  string(5) "hello"
  ["second"]=>
  int(42)
}
object(CustomBox<string>)#%d (1) {
  ["value"]=>
  string(5) "world"
}
Plain: O:5:"Plain":1:{s:1:"x";i:99;}
object(Plain)#%d (1) {
  ["x"]=>
  int(99)
}
OK
