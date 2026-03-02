--TEST--
GH-16261 (Reference invariant broken in mb_convert_variables())
--EXTENSIONS--
mbstring
--FILE--
<?php
class Test {
    public string $x;
    public string $y;
    public array $z;
}
$test = new Test;
$ref = "hello";
$ref2 = "world";
$ref3 = [&$ref2];
$test->x =& $ref;
$test->z =& $ref3;
mb_convert_variables("EUC-JP", "Shift_JIS", $test);

class Test2 {
    public function __construct(public string $x) {}
}
$test2 = new Test2("foo");

mb_convert_variables("EUC-JP", "Shift_JIS", $test->x);

var_dump($test, $test2);
?>
--EXPECT--
object(Test)#1 (2) {
  ["x"]=>
  string(5) "hello"
  ["y"]=>
  uninitialized(string)
  ["z"]=>
  &array(1) {
    [0]=>
    string(5) "world"
  }
}
object(Test2)#2 (1) {
  ["x"]=>
  string(3) "foo"
}
