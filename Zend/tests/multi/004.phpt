--TEST--
union of simple types
--FILE--
<?php
$cb = function(callable | array $arg) {
	return $arg;
};

var_dump($cb($cb), $cb(["just an array"]));
?>
--EXPECT--
object(Closure)#1 (1) {
  ["parameter"]=>
  array(1) {
    ["$arg"]=>
    string(10) "<required>"
  }
}
array(1) {
  [0]=>
  string(13) "just an array"
}
