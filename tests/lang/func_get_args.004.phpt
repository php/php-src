--TEST--
Pass same variable by ref and by value. 
--FILE--
<?php
function valRef($x, &$y) {
	var_dump($x, $y);
	var_dump(func_get_args());
	$x = 'changed.x';
	$y = 'changed.y';
	var_dump(func_get_args());
}

function refVal(&$x, $y) {
	var_dump($x, $y);
	var_dump(func_get_args());
	$x = 'changed.x';
	$y = 'changed.y';
	var_dump(func_get_args());
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
array(2) {
  [0]=>
  string(10) "original.a"
  [1]=>
  string(10) "original.a"
}
array(2) {
  [0]=>
  string(10) "original.a"
  [1]=>
  string(9) "changed.y"
}
string(9) "changed.y"


-- Ref, Val --
string(10) "original.b"
string(10) "original.b"
array(2) {
  [0]=>
  string(10) "original.b"
  [1]=>
  string(10) "original.b"
}
array(2) {
  [0]=>
  string(9) "changed.x"
  [1]=>
  string(10) "original.b"
}
string(9) "changed.x"