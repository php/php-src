--TEST--
SPL: ArrayIterator with NULL key
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php

$ar = new ArrayIterator(array(NULL=>NULL));
@var_dump($ar);
var_dump($ar->getArrayCopy());

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
object(ArrayIterator)#%d (1) {
  [""]=>
  NULL
}
array(1) {
  [""]=>
  NULL
}
===DONE===
