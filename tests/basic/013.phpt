--TEST--
POST Method test and arrays
--POST--
a[]=1
--FILE--
<?php
var_dump($_POST['a']);
?>
--EXPECT--
array(1) {
  [0]=>
  string(1) "1"
}
