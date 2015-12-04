--TEST--
Bug #71028 (Undefined index with ArrayIterator)
--FILE--
<?php
function cast(&$a) {
		$a = (int)$a;
}

$a = new ArrayIterator;
$a[-1] = 123;

$b = "-1";
cast($b);

var_dump(isset($a[$b]));
$a[$b] = "okey";
echo $a[$b];
?>
--EXPECT--
bool(true)
okey
