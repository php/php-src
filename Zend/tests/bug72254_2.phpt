--TEST--
bug #72254 - test with inaccessible properties
--FILE--
<?php
class A {
	public $x = 1;
	protected $y = 2;
	private $z = 3;
};
$a = new A();
$a->{1} = 5;
$b =get_object_vars($a);
var_dump($b, $b[1], $b["1"], $b["x"]);
?>
===DONE===
<?php exit(0); ?>
--EXPECT--
array(2) {
  ["x"]=>
  int(1)
  [1]=>
  int(5)
}
int(5)
int(5)
int(1)
===DONE===
