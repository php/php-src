--TEST--
Default parameters - 11, call_user_func
--FILE--
<?php
function foo($a, $b=1, &$c=2, $d = 3) {
	$c++;
	var_dump($a, $b, $c, $d);
	var_dump(func_num_args());
	var_dump(func_get_args());
	echo "==\n";
}

call_user_func('foo', 5, 6, default, 8);
echo "Done\n";
?>
--EXPECTF--	
int(5)
int(6)
int(3)
int(8)
int(4)
array(4) {
  [0]=>
  int(5)
  [1]=>
  int(6)
  [2]=>
  int(3)
  [3]=>
  int(8)
}
==
Done
