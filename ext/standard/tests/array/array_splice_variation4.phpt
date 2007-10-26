--TEST--
Test array_splice() function : usage variations - non array replacement values
--FILE--
<?php
/* 
 * proto array array_splice(array input, int offset [, int length [, array replacement]])
 * Function is implemented in ext/standard/array.c
*/ 

function test_splice ($replacement)
{
	$input_array=array(0,1);
	var_dump (array_splice ($input_array,2,0,$replacement));
	var_dump ($input_array);
}

test_splice (2);

test_splice (2.1);

test_splice (true);
//file type resource
$file_handle = fopen(__FILE__, "r");

test_splice ($file_handle);
echo "Done\n";
?>
--EXPECTF--
array(0) {
}
array(3) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
}
array(0) {
}
array(3) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  float(2.1)
}
array(0) {
}
array(3) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  bool(true)
}
array(0) {
}
array(3) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  resource(%d) of type (stream)
}
Done