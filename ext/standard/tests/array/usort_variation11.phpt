--TEST--
Test usort() function : usage variations - binary return cmp
--FILE--
<?php

function ucmp($a, $b) {
	return $a > $b;
}

$range = array(2, 4, 8, 16, 32, 64, 128);

foreach ($range as $r) {
	$backup = $array = range(0, $r);
	shuffle($array);
	usort($array, "ucmp");
	if ($array != $backup) {
		var_dump($array);
		var_dump($backup);
		die("Whatever sorting algo you used, this test should never be broken");
	}
}
echo "okey";
?>
--EXPECT--
okey
