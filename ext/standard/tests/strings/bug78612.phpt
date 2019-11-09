--TEST--
Bug #78612 (strtr leaks memory when integer keys are used and the subject string shorter).
--FILE--
<?php

$find_replace = array();
$_a = 7111222333000001;
$_b = 5000001;

for ($j=0; $j<10; $j++) {
	$find_replace[$_a + $j] = $_b + $j;
}

echo strtr('Hello', $find_replace), "\n";

?>
--EXPECT--
Hello
