--TEST--
POST Method test and arrays - 7 
--SKIPIF--
<?php if (php_sapi_name()=='cli') echo 'skip'; ?>
--POST--
a[]=1&a[]]=3&a[[]=4
--FILE--
<?php
var_dump($_POST['a']); 
?>
--EXPECT--
array(3) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "3"
  ["["]=>
  string(1) "4"
}
