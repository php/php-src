--TEST--
Test mb_strtoupper() function : basic functionality
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
 * Test basic functionality of mb_strtoupper
 */

echo "*** Testing mb_strtoupper() : basic functionality ***\n";

mb_internal_encoding('utf-8');
$ascii_lower = 'abcdefghijklmnopqrstuvwxyz';
$ascii_upper = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';
$greek_lower = base64_decode('zrHOss6zzrTOtc62zrfOuM65zrrOu868zr3Ovs6/z4DPgc+Dz4TPhc+Gz4fPiM+J');
$greek_upper = base64_decode('zpHOks6TzpTOlc6WzpfOmM6ZzprOm86czp3Ons6fzqDOoc6jzqTOpc6mzqfOqM6p');

echo "\n-- ASCII String --\n";
$ascii = mb_strtoupper($ascii_lower);
var_dump($ascii);

if($ascii == $ascii_upper) {
	echo "Correctly converted\n";
} else {
	echo "Incorrectly converted\n";
}

echo "\n-- Multibyte String --\n";
$mb = mb_strtoupper($greek_lower, 'UTF-8');
var_dump(base64_encode($mb));

if ($mb == $greek_upper) {
	echo "Correctly converted\n";
} else {
	echo "Incorreclty converted\n";
}

echo "Done";
?>

--EXPECTF--
*** Testing mb_strtoupper() : basic functionality ***

-- ASCII String --
string(26) "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
Correctly converted

-- Multibyte String --
string(64) "zpHOks6TzpTOlc6WzpfOmM6ZzprOm86czp3Ons6fzqDOoc6jzqTOpc6mzqfOqM6p"
Correctly converted
Done