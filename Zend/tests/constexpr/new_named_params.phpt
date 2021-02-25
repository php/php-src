--TEST--
Named params in new in const expr (not supported yet)
--FILE--
<?php

class Vec {
    public function __construct(public float $x, public float $y, public float $z) {}
}

const A = new Vec(x: 0.0, y: 1.0, z: 2.0);
var_dump(A);

const B = new Vec(z: 0.0, y: 1.0, x: 2.0);
var_dump(B);

const C = new Vec(0.0, z: 1.0, y: 2.0);
var_dump(C);

try {
    eval('const D = new Vec(x: 0.0, x: 1.0);');
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
object(Vec)#1 (3) {
  ["x"]=>
  float(0)
  ["y"]=>
  float(1)
  ["z"]=>
  float(2)
}
object(Vec)#2 (3) {
  ["x"]=>
  float(2)
  ["y"]=>
  float(1)
  ["z"]=>
  float(0)
}
object(Vec)#3 (3) {
  ["x"]=>
  float(0)
  ["y"]=>
  float(2)
  ["z"]=>
  float(1)
}
Named parameter $x overwrites previous argument
