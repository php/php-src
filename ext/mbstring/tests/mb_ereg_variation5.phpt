--TEST--
Test mb_ereg() function : usage variations - Test anchors in regex
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
 * Test mb_ereg with anchors (start and end of string) in $pattern
 */

echo "*** Testing mb_ereg() : usage variations ***\n";

mb_regex_encoding('utf-8');

$string_ascii = 'This is an English string. 0123456789.';
$string_mb = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII=');

$regex = '^.*?[[:blank:]]?[[:punct:][:digit:]]+\.?$';

echo "\nASCII String without \$regs arg:\t\t";
var_dump(mb_ereg($regex, $string_ascii));
echo "ASCII String with \$regs arg:\n";
var_dump(mb_ereg($regex, $string_ascii, $regs_ascii));
base64_encode_var_dump($regs_ascii);

echo "\nMultibyte String without \$regs arg:\t";
var_dump(mb_ereg($regex, $string_mb));
echo "Multubyte String with \$regs arg:\n";
var_dump(mb_ereg($regex, $string_mb, $regs_mb));
base64_encode_var_dump($regs_mb);

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
*** Testing mb_ereg() : usage variations ***

ASCII String without $regs arg:		int(1)
ASCII String with $regs arg:
int(38)
array(1) {
  [0]=>
  string(52) "VGhpcyBpcyBhbiBFbmdsaXNoIHN0cmluZy4gMDEyMzQ1Njc4OS4="
}

Multibyte String without $regs arg:	int(1)
Multubyte String with $regs arg:
int(53)
array(1) {
  [0]=>
  string(72) "5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII="
}
Done
