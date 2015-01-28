--TEST--
Default parameters - 2
--FILE--
<?php
function foo($a, $b=1, $c=2, $d = 3) {
	var_dump($a, $b, $c, $d);
	var_dump(func_num_args());
	var_dump(func_get_args());
}

foo(1,2,3);
foo(1,default,3);
foo(default,2,3);
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
int(1)
int(1)
int(3)
int(3)
int(3)
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [2]=>
  int(3)
}

Fatal error: Defaults can be used only for declared optional parameters in %s line %d
