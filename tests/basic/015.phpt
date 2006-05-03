--TEST--
POST Method test and arrays - 3 
--SKIPIF--
<?php if (php_sapi_name()=='cli') echo 'skip'; ?>
--POST--
a[]=1&a[0]=5
--FILE--
<?php
var_dump($_POST['a']); 
?>
--EXPECT--
array(1) {
  [0]=>
  string(1) "5"
}
