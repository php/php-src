--TEST--
Test substr_count() function
--FILE--
<?php
/* Prototype: int substr_count ( string $haystack, string $needle [, int $offset [, int $length]] );
 * Description: substr_count() returns the number of times the needle substring occurs in the 
 *               haystack string. Please note that needle is case sensitive
 */

/* Count the number of substring occurrences */
echo "***Testing basic operations ***\n";
var_dump(@substr_count("", ""));
var_dump(@substr_count("a", ""));
var_dump(@substr_count("", "a"));
var_dump(@substr_count("", "a"));
var_dump(@substr_count("", chr(0)));
$a = str_repeat("abcacba", 100);
var_dump(@substr_count($a, "bca"));
$a = str_repeat("abcacbabca", 100);
var_dump(@substr_count($a, "bca"));
var_dump(substr_count($a, "bca", 200));
var_dump(substr_count($a, "bca", 200, 50));

echo "\n*** Testing possible variations ***\n";
echo "-- 3rd or 4th arg as string --\n";
$str = "this is a string";
var_dump( substr_count($str, "t", "5") );
var_dump( substr_count($str, "t", "5", "10") );
var_dump( substr_count($str, "i", "5t") );
var_dump( substr_count($str, "i", "5t", "10t") );

echo "\n-- 3rd or 4th arg as NULL --\n";
var_dump( substr_count($str, "t", "") );
var_dump( substr_count($str, "T", "") );
var_dump( substr_count($str, "t", "", 15) );
var_dump( substr_count($str, "I", NULL) );
var_dump( substr_count($str, "i", NULL, 10) );

echo "\n-- overlapped substrings --\n";
var_dump( substr_count("abcabcabcabcabc", "abca") ); 
var_dump( substr_count("abcabcabcabcabc", "abca", 2) ); 

echo "\n-- complex strings containing other than 7-bit chars --\n";
$str = chr(128).chr(129).chr(128).chr(256).chr(255).chr(254).chr(255);
var_dump(substr_count($str, chr(128)));
var_dump(substr_count($str, chr(255)));
var_dump(substr_count($str, chr(256)));

echo "\n-- heredoc string --\n";
$string = <<<EOD
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
acdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
acdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
abcdefghijklmnopqrstuvwxyz0123456789abcdefghijklmnopqrstuvwxyz0123456789
EOD;
var_dump(substr_count($string, "abcd"));
var_dump(substr_count($string, "1234"));

echo "\n-- heredoc null string --\n";
$str = <<<EOD
EOD;
var_dump(substr_count($str, "\0"));
var_dump(substr_count($str, "\x000"));
var_dump(substr_count($str, "0"));


echo "\n*** Testing error conditions ***\n";
/* Zero argument */
var_dump( substr_count() );

/* more than expected no. of args */
var_dump( substr_count($str, "t", 0, 15, 30) );
	
/* offset as negative value */
var_dump(substr_count($str, "t", -5));

/* offset > size of the string */
var_dump(substr_count($str, "t", 25));

/* Using offset and length to go beyond the size of the string: 
   Warning message expected, as length+offset > length of string */
var_dump( substr_count($str, "i", 5, 15) );

/* length as Null */
var_dump( substr_count($str, "t", "", "") );
var_dump( substr_count($str, "i", NULL, NULL) );
	
echo "Done\n";	

?>
--EXPECTF--
***Testing basic operations ***
bool(false)
bool(false)
int(0)
int(0)
int(0)
int(100)
int(200)
int(160)
int(10)

*** Testing possible variations ***
-- 3rd or 4th arg as string --
int(1)
int(1)
int(2)
int(2)

-- 3rd or 4th arg as NULL --
int(2)
int(0)
int(2)
int(0)
int(2)

-- overlapped substrings --
int(2)
int(2)

-- complex strings containing other than 7-bit chars --
int(2)
int(2)
int(1)

-- heredoc string --
int(14)
int(16)

-- heredoc null string --
int(0)
int(0)
int(0)

*** Testing error conditions ***

Warning: Wrong parameter count for substr_count() in %s on line %d
NULL

Warning: Wrong parameter count for substr_count() in %s on line %d
NULL

Warning: substr_count(): Offset should be greater than or equal to 0. in %s on line %d
bool(false)

Warning: substr_count(): Offset value 25 exceeds string length. in %s on line %d
bool(false)

Warning: substr_count(): Offset value 5 exceeds string length. in %s on line %d
bool(false)

Warning: substr_count(): Length should be greater than 0. in %s on line %d
bool(false)

Warning: substr_count(): Length should be greater than 0. in %s on line %d
bool(false)
Done
