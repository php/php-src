--TEST--
Test assignment to typed reference with weak type conversion
--FILE--
<?php
  
class Test {
    public string $x = "x";
}

$test = new Test;
var_dump($test);
$y = "y";
$test->x = &$y;
var_dump($y, $test);

$z = 42;
$y = $z;
var_dump($y, $z, $test);

?>
--EXPECT--
object(Test)#1 (1) {
  ["x"]=>
  string(1) "x"
}
string(1) "y"
object(Test)#1 (1) {
  ["x"]=>
  &string(1) "y"
}
string(2) "42"
int(42)
object(Test)#1 (1) {
  ["x"]=>
  &string(2) "42"
}
