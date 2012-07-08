--TEST--
Default parameters - 3
--FILE--
<?php
function test($a=1, $b=2, $c=3) {
	var_dump(func_get_args());
	var_dump(func_get_arg(1));
}

test(7,default);
echo "Done\n";
?>
--EXPECTF--	
array(1) {
  [0]=>
  int(7)
}
bool(false)
Done
