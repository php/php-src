--TEST--
Test mb_ereg() function : basic functionality 
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_ereg') or die("skip mb_ereg() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : int mb_ereg(string $pattern, string $string [, array $registers])
 * Description: Regular expression match for multibyte string 
 * Source code: ext/mbstring/php_mbregex.c
 */

/*
 * Test basic functionality of mb_ereg
 */

echo "*** Testing mb_ereg() : basic functionality ***\n";

if(mb_regex_encoding('utf-8') == true) {
	echo "Regex encoding set to utf-8\n";
} else {
	echo "Could not set regex encoding to utf-8\n";
}
$string_ascii = b'This is an English string. 0123456789.';
$regex_ascii1 = b'(.*is)+.*\.[[:blank:]][0-9]{9}';
$regex_ascii2 = b'.*is+';

$string_mb = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII=');
$regex_mb1 = base64_decode('KOaXpeacrOiqnikuKj8oWzEtOV0rKQ==');
$regex_mb2 = base64_decode('5LiW55WM');

echo "\n**-- ASCII String --**\n";
echo "-- Without \$regs argument--\n";
var_dump(mb_ereg($regex_ascii1, $string_ascii));
var_dump(mb_ereg($regex_ascii2, $string_ascii));
echo "--With \$regs argument --\n";
var_dump(mb_ereg($regex_ascii1, $string_ascii, $regs_ascii1));
base64_encode_var_dump($regs_ascii1);
var_dump(mb_ereg($regex_ascii2, $string_ascii, $regs_ascii2));
base64_encode_var_dump($regs_ascii2);

echo "\n**-- Multibyte String --**\n";
echo "-- Without \$regs argument --\n";
var_dump(mb_ereg($regex_mb1, $string_mb));
var_dump(mb_ereg($regex_mb2, $string_mb));
echo "-- With \$regs argument --\n";
var_dump(mb_ereg($regex_mb1, $string_mb, $regs_mb1));
base64_encode_var_dump($regs_mb1);
var_dump(mb_ereg($regex_mb2, $string_mb, $regs_mb2));
var_dump($regs_mb2);

echo "Done";

/**
 * replicate a var dump of an array but outputted string values are base64 encoded
 *
 * @param array $regs
 */
function base64_encode_var_dump($regs) {
	if ($regs) {
		echo "array(" . count($regs) . ") {\n";
		foreach ($regs as $key => $value) {
			echo "  [$key]=>\n  ";
			if (is_string($value)) {
				var_dump(base64_encode($value));
			} else {
				var_dump($value);
			}
		}
		echo "}\n";
	} else {
		echo "NULL\n";
	}
}
?>

--EXPECT--
*** Testing mb_ereg() : basic functionality ***
Regex encoding set to utf-8

**-- ASCII String --**
-- Without $regs argument--
int(1)
int(1)
--With $regs argument --
int(36)
array(2) {
  [0]=>
  string(48) "VGhpcyBpcyBhbiBFbmdsaXNoIHN0cmluZy4gMDEyMzQ1Njc4"
  [1]=>
  string(24) "VGhpcyBpcyBhbiBFbmdsaXM="
}
int(17)
array(1) {
  [0]=>
  string(24) "VGhpcyBpcyBhbiBFbmdsaXM="
}

**-- Multibyte String --**
-- Without $regs argument --
int(1)
bool(false)
-- With $regs argument --
int(35)
array(3) {
  [0]=>
  string(48) "5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzQ="
  [1]=>
  string(12) "5pel5pys6Kqe"
  [2]=>
  string(8) "MTIzNA=="
}
bool(false)
NULL
Done