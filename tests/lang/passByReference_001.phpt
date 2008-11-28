--TEST--
passing of function parameters by reference
--FILE--
<?php
function f($arg1, &$arg2)
{
	var_dump($arg1++);
	var_dump($arg2++);
}

function g (&$arg1, &$arg2)
{
	var_dump($arg1);
	var_dump($arg2);
}
$a = 7;
$b = 15;

f($a, $b);

var_dump($a);
var_dump($b);

$c=array(1);
g($c,$c[0]);

?>
--EXPECT--
int(7)
int(15)
int(7)
int(16)
array(1) {
  [0]=>
  &int(1)
}
int(1)