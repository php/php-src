--TEST--
POST Method test and arrays - 5 
--POST--
a[]=1&a[a]=1&a[b]=3
--FILE--
<?php
var_dump($_POST['a']); 
?>
--EXPECT--
array(3) {
  [0]=>
  string(1) "1"
  ["a"]=>
  string(1) "1"
  ["b"]=>
  string(1) "3"
}
