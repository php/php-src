--TEST--
Bug #31926 (php in free() error with RecursiveArrayIterator)
--FILE--
<?php

$array = array(0 => array('world'));

$it = new RecursiveIteratorIterator(new RecursiveArrayIterator($array));
foreach($it as $key => $val) {
   var_dump($key, $val);
}

?>
--EXPECT--
int(0)
string(5) "world"
