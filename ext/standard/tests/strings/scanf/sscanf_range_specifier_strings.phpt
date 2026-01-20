--TEST--
sscanf(): test %[] specifier with strings
--FILE--
<?php

$formats = [
	'%[aefgbcd]',
	'%[a-g]',
	'%[g-a]',
	'%[^uvwstxyz]',
	'%[^s-z]',
	'%[^z-s]',
	'%4[aefgbcd]',
	'%4[g-a]',
	'%4[^uvwstxyz]',
	'%4[^s-z]',
	'%4[^z-s]',
];

$str = "abcdefghijklmnopqrstuvwxyz";

foreach ($formats as $format) {
	echo "Using format string '$format':\n";
	sscanf($str, $format, $out);
	var_dump($out);
}
?>
--EXPECT--
Using format string '%[aefgbcd]':
string(7) "abcdefg"
Using format string '%[a-g]':
string(7) "abcdefg"
Using format string '%[g-a]':
string(7) "abcdefg"
Using format string '%[^uvwstxyz]':
string(18) "abcdefghijklmnopqr"
Using format string '%[^s-z]':
string(18) "abcdefghijklmnopqr"
Using format string '%[^z-s]':
string(18) "abcdefghijklmnopqr"
Using format string '%4[aefgbcd]':
string(4) "abcd"
Using format string '%4[g-a]':
string(4) "abcd"
Using format string '%4[^uvwstxyz]':
string(4) "abcd"
Using format string '%4[^s-z]':
string(4) "abcd"
Using format string '%4[^z-s]':
string(4) "abcd"
