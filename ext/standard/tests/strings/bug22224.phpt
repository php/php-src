--TEST--
Bug #22224 (implode changes object references in array)
--INI--
error_reporting=0
--FILE--
<?php
class foo {
}


$a = new foo();
			    
$arr = array(0=>&$a, 1=>&$a);
var_dump(implode(",",$arr));
var_dump($arr)
?>
--EXPECT--
string(13) "Object,Object"
array(2) {
  [0]=>
  &object(foo)(0) {
  }
  [1]=>
  &object(foo)(0) {
  }
}
