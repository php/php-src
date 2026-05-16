--TEST--
imageaffinematrixget
--EXTENSIONS--
gd
--FILE--
<?php
class A{}
$a = 10;
try {
	imageaffinematrixget(IMG_AFFINE_ROTATE, new A());
} catch (\TypeError $e) {
	echo $e->getMessage(), PHP_EOL;
}
try {
	imageaffinematrixget(IMG_AFFINE_SCALE, ["x" => new A(), "y" => 1]);
} catch (\TypeError $e) {
	echo $e->getMessage(), PHP_EOL;
}
try {
	imageaffinematrixget(IMG_AFFINE_SCALE, ["x" => 10, "y" => new A()]);
} catch (\TypeError $e) {
	echo $e->getMessage(), PHP_EOL;
}
var_dump(imageaffinematrixget(IMG_AFFINE_SCALE, ["x" => &$a, "y" => &$a]));
?>
--EXPECTF--

Warning: Object of class A could not be converted to float in %s on line %d

Warning: Object of class A could not be converted to float in %s on line %d

Warning: Object of class A could not be converted to float in %s on line %d
array(6) {
  [0]=>
  float(10)
  [1]=>
  float(0)
  [2]=>
  float(0)
  [3]=>
  float(10)
  [4]=>
  float(0)
  [5]=>
  float(0)
}
