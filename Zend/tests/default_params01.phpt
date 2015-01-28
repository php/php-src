--TEST--
Default parameters - 1
--FILE--
<?php
function test($a=1, $b=2, $c=3) {
	var_dump($a, $b, $c);
	var_dump(func_get_args());
}
echo "7\n";
test(7);
echo "default,4,5\n";
test(default,4,5);
echo "default,default,8\n";
test(default,default,8);
echo "3,default,5\n";
test(3,default,5);
echo "3,default,7,default,6\n";
test(3,default,7,default,6);
?>
--EXPECTF--	
7
int(7)
int(2)
int(3)
array(1) {
  [0]=>
  int(7)
}
default,4,5
int(1)
int(4)
int(5)
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(4)
  [2]=>
  int(5)
}
default,default,8
int(1)
int(2)
int(8)
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(8)
}
3,default,5
int(3)
int(2)
int(5)
array(3) {
  [0]=>
  int(3)
  [1]=>
  int(2)
  [2]=>
  int(5)
}
3,default,7,default,6

Fatal error: Defaults can be used only for declared optional parameters in %s line %d
