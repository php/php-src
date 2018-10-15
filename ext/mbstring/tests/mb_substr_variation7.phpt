--TEST--
Test mb_substr() function : usage variations - pass different integers to $length arg
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_substr') or die("skip mb_substr() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_substr(string $str, int $start [, int $length [, string $encoding]])
 * Description: Returns part of a string
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Test how mb_substr() behaves when passed a range of integers as $length argument
 */

echo "*** Testing mb_substr() : usage variations ***\n";

mb_internal_encoding('UTF-8');

$string_ascii = '+Is an English string'; //21 chars

//Japanese string, 21 characters
$string_mb = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII=');

/*
 * Loop through integers as multiples of ten for $offset argument
 * 60 is larger than *BYTE* count for $string_mb
 */
for ($i = -60; $i <= 60; $i += 10) {
	if (@$a || @$b) {
		$a = null;
		$b = null;
	}
	echo "\n**-- Offset is: $i --**\n";
	echo "-- ASCII String --\n";
	$a = mb_substr($string_ascii, 1, $i);
	if ($a !== false) {
	   var_dump(bin2hex($a));
	}
	else {
	   var_dump($a);
	}
	echo "--Multibyte String --\n";
	$b = mb_substr($string_mb, 1, $i, 'UTF-8');
	if (strlen($a) == mb_strlen($b, 'UTF-8')) { // should return same length
		var_dump(bin2hex($b));
	} else {
		echo "Difference in length of ASCII string and multibyte string\n";
	}

}

echo "Done";
?>
--EXPECT--
*** Testing mb_substr() : usage variations ***

**-- Offset is: -60 --**
-- ASCII String --
string(0) ""
--Multibyte String --
string(0) ""

**-- Offset is: -50 --**
-- ASCII String --
string(0) ""
--Multibyte String --
string(0) ""

**-- Offset is: -40 --**
-- ASCII String --
string(0) ""
--Multibyte String --
string(0) ""

**-- Offset is: -30 --**
-- ASCII String --
string(0) ""
--Multibyte String --
string(0) ""

**-- Offset is: -20 --**
-- ASCII String --
string(0) ""
--Multibyte String --
string(0) ""

**-- Offset is: -10 --**
-- ASCII String --
string(20) "497320616e20456e676c"
--Multibyte String --
string(56) "e69cace8aa9ee38386e382ade382b9e38388e381a7e38199e3808230"

**-- Offset is: 0 --**
-- ASCII String --
string(0) ""
--Multibyte String --
string(0) ""

**-- Offset is: 10 --**
-- ASCII String --
string(20) "497320616e20456e676c"
--Multibyte String --
string(56) "e69cace8aa9ee38386e382ade382b9e38388e381a7e38199e3808230"

**-- Offset is: 20 --**
-- ASCII String --
string(40) "497320616e20456e676c69736820737472696e67"
--Multibyte String --
string(100) "e69cace8aa9ee38386e382ade382b9e38388e381a7e38199e380823031323334efbc95efbc96efbc97efbc98efbc99e38082"

**-- Offset is: 30 --**
-- ASCII String --
string(40) "497320616e20456e676c69736820737472696e67"
--Multibyte String --
string(100) "e69cace8aa9ee38386e382ade382b9e38388e381a7e38199e380823031323334efbc95efbc96efbc97efbc98efbc99e38082"

**-- Offset is: 40 --**
-- ASCII String --
string(40) "497320616e20456e676c69736820737472696e67"
--Multibyte String --
string(100) "e69cace8aa9ee38386e382ade382b9e38388e381a7e38199e380823031323334efbc95efbc96efbc97efbc98efbc99e38082"

**-- Offset is: 50 --**
-- ASCII String --
string(40) "497320616e20456e676c69736820737472696e67"
--Multibyte String --
string(100) "e69cace8aa9ee38386e382ade382b9e38388e381a7e38199e380823031323334efbc95efbc96efbc97efbc98efbc99e38082"

**-- Offset is: 60 --**
-- ASCII String --
string(40) "497320616e20456e676c69736820737472696e67"
--Multibyte String --
string(100) "e69cace8aa9ee38386e382ade382b9e38388e381a7e38199e380823031323334efbc95efbc96efbc97efbc98efbc99e38082"
Done
