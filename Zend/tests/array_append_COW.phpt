--TEST--
Tests that array manipulation code is correctly dealing with copy on write and splitting on reference
--FILE--
<?php
	$a=array();
	$b=1;
	$c=&$b;
	$a[]=$b;
	$b=2;
	var_dump ($a);
?>
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
