--TEST--
Implicit initialisation when passing by reference
--FILE--
<?php
function passbyVal($val) {
	echo "\nInside passbyVal call:\n";
	var_dump($val);
}

function passbyRef(&$ref) {
	echo "\nInside passbyRef call:\n";
	var_dump($ref);
}

echo "\nPassing undefined by value\n";
passbyVal($undef1[0]);
echo "\nAfter call\n";
var_dump($undef1);

echo "\nPassing undefined by reference\n";
passbyRef($undef2[0]);
echo "\nAfter call\n";
var_dump($undef2)
?>
--EXPECTF--
Passing undefined by value

Notice: Undefined variable: undef1 in %s on line 13

Inside passbyVal call:
NULL

After call

Notice: Undefined variable: undef1 in %s on line 15
NULL

Passing undefined by reference

Inside passbyRef call:
NULL

After call
array(1) {
  [0]=>
  NULL
}
