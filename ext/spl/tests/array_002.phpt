--TEST--
SPL: ArrayObject copy constructor
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php

$array = array('1' => 'one',
               '2' => 'two',
               '3' => 'three');

$object = new ArrayObject($array);
$object[] = 'four';

$arrayObject = new ArrayObject($object);

$arrayObject[] = 'five';

var_dump($arrayObject);

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
object(ArrayObject)#%d (5) {
  [1]=>
  string(3) "one"
  [2]=>
  string(3) "two"
  [3]=>
  string(5) "three"
  [4]=>
  string(4) "four"
  [5]=>
  string(4) "five"
}
===DONE===
