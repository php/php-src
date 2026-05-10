--TEST--
Inherited methods: variadic T parameter is checked element-by-element against the substituted type
--FILE--
<?php
class Box<T> {
    public function take(T ...$args): void {
        var_dump($args);
    }
}

class IntBox extends Box<int> {}

(new IntBox())->take(1, 2, 3);

(new IntBox())->take();

try {
    (new IntBox())->take(1, "hello", 3);
} catch (TypeError $e) {
    echo "mid: ", $e->getMessage(), "\n";
}

try {
    (new IntBox())->take([1]);
} catch (TypeError $e) {
    echo "arr: ", $e->getMessage(), "\n";
}

(new Box())->take(1, "x", [1, 2]);
?>
--EXPECTF--
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
array(0) {
}
mid: Box::take(): Argument #2 must be of type int, string given, called in %s on line %d
arr: Box::take(): Argument #1 must be of type int, array given, called in %s on line %d
array(3) {
  [0]=>
  int(1)
  [1]=>
  string(1) "x"
  [2]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(2)
  }
}
