--TEST--
Bug #34934 (offsetExists is not called from array_key_exists)
--FILE--
<?php
class MyArray extends ArrayObject {
  function offsetExists($mKey) { 
  	echo __METHOD__ . "($mKey)\n";
  	return true;
  }
}

$a = new MyArray();

var_dump(array_key_exists("test", $a));
?>
--EXPECT--
MyArray::offsetExists(test)
bool(true)
