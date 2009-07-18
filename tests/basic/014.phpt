--TEST--
POST Method test and arrays - 2
--POST--
a[]=1&a[]=1
--FILE--
<?php
var_dump($_POST['a']); 
?>
--EXPECT--
array(2) {
  [0]=>
  unicode(1) "1"
  [1]=>
  unicode(1) "1"
}
