--TEST--
Test htmlspecialchars() function : basic functionality 
--FILE--
<?php
/* Prototype  : string htmlspecialchars  ( string $string  [, int $quote_style  [, string $charset  [, bool $double_encode  ]]] )
 * Description: Convert special characters to HTML entities
 * Source code: ext/standard/string.c
*/

echo "*** Testing htmlspecialchars() : basic functionality ***\n";

class Foo {
	function __toString() {
		return 'abcdefg';
	}
}

$s1 = 123456789;
$s2 = -123456789;
$s3 = 1234.5678;
$s4 = -1234.5678;
$s5 = new Foo;
echo "Basic tests\n";
echo "Test 1: " . htmlspecialchars ($s1) . "\n";
echo "Test 2: " . htmlspecialchars ($s2) . "\n";
echo "Test 3: " . htmlspecialchars ($s3) . "\n";
echo "Test 4: " . htmlspecialchars ($s4) . "\n";
echo "Test 5: " . htmlspecialchars ($s5) . "\n";
echo "Test 6: " . htmlspecialchars ($s1,ENT_NOQUOTES) . "\n";
echo "Test 7: " . htmlspecialchars ($s2,ENT_NOQUOTES) . "\n";
echo "Test 8: " . htmlspecialchars ($s3,ENT_NOQUOTES) . "\n";
echo "Test 9: " . htmlspecialchars ($s4,ENT_NOQUOTES) . "\n";
echo "Test 10: " . htmlspecialchars ($s5,ENT_NOQUOTES) . "\n";
echo "Test 11: " . htmlspecialchars ($s1,ENT_COMPAT) . "\n";
echo "Test 12: " . htmlspecialchars ($s2,ENT_COMPAT) . "\n";
echo "Test 13: " . htmlspecialchars ($s3,ENT_COMPAT) . "\n";
echo "Test 14: " . htmlspecialchars ($s4,ENT_COMPAT) . "\n";
echo "Test 15: " . htmlspecialchars ($s5,ENT_COMPAT) . "\n";
echo "Test 16: " . htmlspecialchars ($s1,ENT_QUOTES) . "\n";
echo "Test 17: " . htmlspecialchars ($s2,ENT_QUOTES) . "\n";
echo "Test 18: " . htmlspecialchars ($s3,ENT_QUOTES) . "\n";
echo "Test 19: " . htmlspecialchars ($s4,ENT_QUOTES) . "\n";
echo "Test 20: " . htmlspecialchars ($s5,ENT_QUOTES) . "\n";

echo "\nTry with char set option - specify default ISO-8859-1\n";
echo  "Test 21: " . htmlspecialchars ($s1,ENT_NOQUOTES, "ISO-8859-1") . "\n";
echo  "Test 22: " . htmlspecialchars ($s2,ENT_COMPAT, "ISO-8859-1") . "\n";
echo  "Test 23: " . htmlspecialchars ($s3,ENT_QUOTES, "ISO-8859-1") . "\n";
echo  "Test 24: " . htmlspecialchars ($s5,ENT_QUOTES, "ISO-8859-1") . "\n";

echo "\nTry with with double decode FALSE\n";
echo  "Test 25: " . htmlspecialchars ($s1,ENT_NOQUOTES, "ISO-8859-1", false) . "\n";
echo  "Test 26: " . htmlspecialchars ($s2,ENT_NOQUOTES, "ISO-8859-1", false) . "\n";
echo  "Test 27: " . htmlspecialchars ($s3,ENT_NOQUOTES, "ISO-8859-1", false) . "\n";
echo  "Test 28: " . htmlspecialchars ($s4,ENT_NOQUOTES, "ISO-8859-1", false) . "\n";
echo  "Test 29: " . htmlspecialchars ($s5,ENT_NOQUOTES, "ISO-8859-1", false) . "\n";

echo "\nTry with double decode TRUE\n";
echo  "Test 30: " . htmlspecialchars ($s1, ENT_NOQUOTES, "ISO-8859-1", true) . "\n";
echo  "Test 31: " . htmlspecialchars ($s2, ENT_NOQUOTES, "ISO-8859-1", true) . "\n";
echo  "Test 32: " . htmlspecialchars ($s3, ENT_NOQUOTES, "ISO-8859-1", true) . "\n";
echo  "Test 33: " . htmlspecialchars ($s4, ENT_NOQUOTES, "ISO-8859-1", true) . "\n";
echo  "Test 34: " . htmlspecialchars ($s5, ENT_NOQUOTES, "ISO-8859-1", true) . "\n";

echo "\nTry with resource\n";
$s1 = fopen(__FILE__, 'rb');
echo  "Test 35: " . htmlspecialchars ($s1) . "\n";
echo  "Test 36: " . htmlspecialchars ($s1, ENT_NOQUOTES) . "\n";
echo  "Test 37: " . htmlspecialchars ($s1, ENT_NOQUOTES, "ISO-8859-1") . "\n";
echo  "Test 38: " . htmlspecialchars ($s1, ENT_NOQUOTES, "ISO-8859-1", true) . "\n";

?>
===DONE===
--EXPECT--
*** Testing htmlspecialchars() : basic functionality ***
Basic tests
Test 1: 123456789
Test 2: -123456789
Test 3: 1234.5678
Test 4: -1234.5678
Test 5: abcdefg
Test 6: 123456789
Test 7: -123456789
Test 8: 1234.5678
Test 9: -1234.5678
Test 10: abcdefg
Test 11: 123456789
Test 12: -123456789
Test 13: 1234.5678
Test 14: -1234.5678
Test 15: abcdefg
Test 16: 123456789
Test 17: -123456789
Test 18: 1234.5678
Test 19: -1234.5678
Test 20: abcdefg

Try with char set option - specify default ISO-8859-1
Test 21: 123456789
Test 22: -123456789
Test 23: 1234.5678
Test 24: abcdefg

Try with with double decode FALSE
Test 25: 123456789
Test 26: -123456789
Test 27: 1234.5678
Test 28: -1234.5678
Test 29: abcdefg

Try with double decode TRUE
Test 30: 123456789
Test 31: -123456789
Test 32: 1234.5678
Test 33: -1234.5678
Test 34: abcdefg

Try with resource
Test 35: Resource id #5
Test 36: Resource id #5
Test 37: Resource id #5
Test 38: Resource id #5
===DONE===
