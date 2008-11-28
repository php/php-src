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
unicode(10) "original.a"
unicode(10) "original.a"
array(2) {
  [0]=>
  unicode(10) "original.a"
  [1]=>
  unicode(10) "original.a"
}
array(2) {
  [0]=>
  unicode(10) "original.a"
  [1]=>
  unicode(9) "changed.y"
}
unicode(9) "changed.y"


-- Ref, Val --
unicode(10) "original.b"
unicode(10) "original.b"
array(2) {
  [0]=>
  unicode(10) "original.b"
  [1]=>
  unicode(10) "original.b"
}
array(2) {
  [0]=>
  unicode(9) "changed.x"
  [1]=>
  unicode(10) "original.b"
}
unicode(9) "changed.x"

