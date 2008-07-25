--TEST--
Test mb_strtoupper() function : usage varitations - pass mixed ASCII and non-ASCII strings
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strtoupper') or die("skip mb_strtoupper() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_strtoupper(string $sourcestring [, string $encoding]
 * Description: Returns a uppercased version of $sourcestring
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Pass a Japanese string and a mixed Japanese and ASCII string to mb_strtolower
 * to check correct conversion is occuring (Japanese characters should not be converted).
 */

echo "*** Testing mb_strtoupper() : usage variations ***\n";

$string_mixed_upper = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCUEhQLiAwMTIzNO+8le+8lu+8l++8mO+8meOAgg==');
$string_mixed_lower = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCcGhwLiAwMTIzNO+8le+8lu+8l++8mO+8meOAgg==');
$string_all_mb = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC');


echo "\n-- Mixed string (mulitbyte and ASCII characters) --\n";
$a = mb_strtoupper($string_mixed_lower, 'UTF-8');
var_dump(base64_encode($a));
if ($a == $string_mixed_upper) {
	echo "Correctly Converted\n";
} else {
	echo "Incorrectly Converted\n";
}

echo "\n-- Multibyte Only String--\n";
$b = mb_strtoupper($string_all_mb, 'UTF-8');
var_dump(base64_encode($b));
if ($b == $string_all_mb) { // Japanese characters only - should not be any conversion
	echo "Correctly Converted\n";
} else {
	echo "Incorrectly Converted\n";
}

echo "Done";
?>

--EXPECTF--
*** Testing mb_strtoupper() : usage variations ***

-- Mixed string (mulitbyte and ASCII characters) --
string(80) "5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCUEhQLiAwMTIzNO+8le+8lu+8l++8mO+8meOAgg=="
Correctly Converted

-- Multibyte Only String--
string(40) "5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC"
Correctly Converted
Done
