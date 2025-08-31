--TEST--
The default value must be legal for one of the type in the union
--FILE--
<?php

class Test {
    public int|float $a = 1;
    public int|float $b = 2.0;
    public float|string $c = 3; // Strict typing exception
    public float|string $d = 4.0;
    public float|string $e = "5";
}

function test(
    int|float $a = 1,
    int|float $b = 2.0,
    float|string $c = 3, // Strict typing exception
    float|string $d = 4.0,
    float|string $e = "5"
) {
    var_dump($a, $b, $c, $d, $e);
}

var_dump(new Test);
test();

?>
--EXPECT--
object(Test)#1 (5) {
  ["a"]=>
  int(1)
  ["b"]=>
  float(2)
  ["c"]=>
  float(3)
  ["d"]=>
  float(4)
  ["e"]=>
  string(1) "5"
}
int(1)
float(2)
float(3)
float(4)
string(1) "5"
