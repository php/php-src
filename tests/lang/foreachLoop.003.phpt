--TEST--
Foreach loop tests - error case: not an array.
--FILE--
<?php
echo "\nNot an array.\n";
$a = TRUE;
foreach ($a as $v) {
	var_dump($v);
}

$a = null;
foreach ($a as $v) {
	var_dump($v);
}

$a = 1;
foreach ($a as $v) {
	var_dump($v);
}

$a = 1.5;
foreach ($a as $v) {
	var_dump($v);
}

$a = "hello";
foreach ($a as $v) {
	var_dump($v);
}

echo "done.\n";
?>
--EXPECTF--
Not an array.

Warning: Invalid argument supplied for foreach() in %s on line 4

Warning: Invalid argument supplied for foreach() in %s on line 9

Warning: Invalid argument supplied for foreach() in %s on line 14

Warning: Invalid argument supplied for foreach() in %s on line 19

Warning: Invalid argument supplied for foreach() in %s on line 24
done.
