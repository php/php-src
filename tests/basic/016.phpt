--TEST--
POST Method test and arrays - 4 
--SKIPIF--
<?php if (php_sapi_name()=='cli') echo 'skip'; ?>
--POST--
a[a]=1&a[b]=3
--FILE--
<?php
var_dump($_POST['a']); 
?>
--EXPECT--
array(2) {
  ["a"]=>
  string(1) "1"
  ["b"]=>
  string(1) "3"
}
