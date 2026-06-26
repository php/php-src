--TEST--
Trait methods: separate using classes get independent substituted views with no cross-contamination
--FILE--
<?php
trait Pair<A, B> {
    public function take(A $a, B $b): void {
        var_dump([$a, $b]);
    }
    public function makeA(): A { return [0]; }
}

class IntStr { use Pair<int, string>; }
class ArrStr { use Pair<array, string>; }

(new IntStr())->take(1, "x");
(new ArrStr())->take([1], "x");

try {
    (new IntStr())->take([1], "x");
} catch (TypeError $e) {
    echo "intstr-a: ", $e->getMessage(), "\n";
}

try {
    (new ArrStr())->take("nope", "x");
} catch (TypeError $e) {
    echo "arrstr-a: ", $e->getMessage(), "\n";
}

var_dump((new ArrStr())->makeA());

try {
    (new IntStr())->makeA();
} catch (TypeError $e) {
    echo "intstr-make: ", $e->getMessage(), "\n";
}
?>
--EXPECTF--
array(2) {
  [0]=>
  int(1)
  [1]=>
  string(1) "x"
}
array(2) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
  [1]=>
  string(1) "x"
}
intstr-a: IntStr::take(): Argument #1 ($a) must be of type int, array given, called in %s on line %d
arrstr-a: ArrStr::take(): Argument #1 ($a) must be of type array, string given, called in %s on line %d
array(1) {
  [0]=>
  int(0)
}
intstr-make: IntStr::makeA(): Return value must be of type int, array returned
