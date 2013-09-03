--TEST--
Default parameters - 4
--FILE--
<?php
function foo($a, $b=1, $c=2, $d = 3) {
	var_dump($a, $b, $c, $d);
	var_dump(func_num_args());
	var_dump(func_get_args());
	echo "==\n";
}

call_user_func('foo', 1,default,2,3);
call_user_func_array('foo', array(42, 2=>43, 4=>44));
echo "Done\n";
?>
--EXPECTF--	
int(1)
int(2)
int(3)
int(3)
int(3)
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
==
int(42)
int(1)
int(43)
int(3)
int(5)
array(3) {
  [0]=>
  int(42)
  [2]=>
  int(43)
  [4]=>
  int(44)
}
==
Done
