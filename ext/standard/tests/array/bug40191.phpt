--TEST--
Bug #40191 (use of array_unique() with objects triggers segfault)
--SKIPIF--
<?php if (!extension_loaded('spl')) die("skip SPL is not available"); ?>
--FILE--
<?php

$arrObj = new ArrayObject();
$arrObj->append('foo');
$arrObj->append('bar');
$arrObj->append('foo');

$arr = array_unique($arrObj);
var_dump($arr);

echo "Done\n";
?>
--EXPECTF--	
array(2) {
  [0]=>
  string(3) "foo"
  [1]=>
  string(3) "bar"
}
Done
