--TEST--
Default parameters - 4, call_user_func
--FILE--
<?php
function foo($a, $b=1, $c=2, $d = 3) {
	var_dump($a, $b, $c, $d);
	var_dump(func_num_args());
	var_dump(func_get_args());
	echo "==\n";
}

call_user_func('foo', 1,default,5,6);
call_user_func('foo', 0,default,7,default);
//call_user_func_array('foo', array(42, 2=>43, 4=>44));
echo "Done\n";
?>
--EXPECTF--	
int(1)
int(1)
int(5)
int(6)
int(4)
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [2]=>
  int(5)
  [3]=>
  int(6)
}
==
int(0)
int(1)
int(7)
int(3)
int(4)
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(7)
  [3]=>
  int(3)
}
==
Done
