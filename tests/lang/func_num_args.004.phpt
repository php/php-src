--TEST--
Pass same variable by ref and by value. 
--FILE--
<?php
function valRef($x, &$y) {
	var_dump($x, $y);
	var_dump(func_num_args());
	$x = 'changed.x';
	$y = 'changed.y';
	var_dump(func_num_args());
}

function refVal(&$x, $y) {
	var_dump($x, $y);
	var_dump(func_num_args());
	$x = 'changed.x';
	$y = 'changed.y';
	var_dump(func_num_args());
}


echo "\n\n-- Val, Ref --\n";
$a = 'original.a';
valRef($a, $a);
var_dump($a);

echo "\n\n-- Ref, Val --\n";
$b = 'original.b';
refVal($b, $b);
var_dump($b);
?>
--EXPECTF--


-- Val, Ref --
string(10) "original.a"
string(10) "original.a"
int(2)
int(2)
string(9) "changed.y"


-- Ref, Val --
string(10) "original.b"
string(10) "original.b"
int(2)
int(2)
string(9) "changed.x"
