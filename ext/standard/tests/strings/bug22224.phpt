--TEST--
Bug #22224 (implode changes object references in array)
--INI--
error_reporting=0
--FILE--
<?php
function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	//ignore all errors
}

set_error_handler('test_error_handler');
	
class foo {
}

$a = new foo();
			    
$arr = array(0=>&$a, 1=>&$a);
var_dump(implode(",",$arr));
var_dump($arr)
?>
--EXPECTF--
string(13) "Object,Object"
array(2) {
  [0]=>
  &object(foo)#%d (0) {
  }
  [1]=>
  &object(foo)#%d (0) {
  }
}
--UEXPECTF--
unicode(13) "Object,Object"
array(2) {
  [0]=>
  &object(foo)#%d (0) {
  }
  [1]=>
  &object(foo)#%d (0) {
  }
}
