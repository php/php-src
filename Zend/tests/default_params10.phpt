--TEST--
Default parameters - 10, shutdown func
--FILE--
<?php
function test($a=1, $b=2, $c=3) {
	var_dump($a, $b, $c);
	var_dump(func_get_args());
}
register_shutdown_function("test", 3, default, 1);
echo "Done\n";
?>
--EXPECTF--	
Done
int(3)
int(2)
int(1)
array(3) {
  [0]=>
  int(3)
  [1]=>
  int(2)
  [2]=>
  int(1)
}
