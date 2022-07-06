--TEST--
Test mb_strtolower() function : usage variations - pass mixed ASCII and non-ASCII strings
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strtolower') or die("skip mb_strtolower() is not available in this build");
?>
--FILE--
<?php
/*
 * Pass a Japanese string and a mixed Japanese and ASCII string to mb_strtolower
 * to check correct conversion is occurring (Japanese characters should not be converted).
 */

echo "*** Testing mb_strtolower() : usage variations ***\n";

$string_mixed = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCUEhQLiAwMTIzNO+8le+8lu+8l++8mO+8meOAgg==');
$string_mixed_lower = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCcGhwLiAwMTIzNO+8le+8lu+8l++8mO+8meOAgg==');
$string_all_mb = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC');

echo "\n-- Mixed string (mulitbyte and ASCII characters) --\n";
$a = mb_strtolower($string_mixed, 'UTF-8');
var_dump(base64_encode($a));
if ($a == $string_mixed_lower) {
    echo "Correctly Converted\n";
} else {
    echo "Incorrectly Converted\n";
}

echo "\n-- Multibyte Only String--\n";
$b = mb_strtolower($string_all_mb, 'UTF-8');
var_dump(base64_encode($b));
if ($b == $string_all_mb) { // Japanese characters only - should not be any conversion
    echo "Correctly Converted\n";
} else {
    echo "Incorrectly Converted\n";
}

echo "Done";
?>
--EXPECT--
*** Testing mb_strtolower() : usage variations ***

-- Mixed string (mulitbyte and ASCII characters) --
string(80) "5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCcGhwLiAwMTIzNO+8le+8lu+8l++8mO+8meOAgg=="
Correctly Converted

-- Multibyte Only String--
string(40) "5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC"
Correctly Converted
Done
