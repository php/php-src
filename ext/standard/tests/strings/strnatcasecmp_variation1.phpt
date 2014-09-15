--TEST--
Test strnatcasecmp() function : variation 
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>
--FILE--
<?php
/* Prototype  : int strnatcasecmp(string s1, string s2)
 * Description: Returns the result of case-insensitive string comparison using 'natural' algorithm 
 * Source code: ext/standard/string.c
 * Alias to functions: 
 */

/* Preparation */
class a
{
	function __toString()
	{
		return "Hello WORLD";
	}
}

class b
{
	function __toString()
	{
		return "HELLO world";
	}
}

$a = new a();
$b = new b();

function str_dump($a, $b) {
	var_dump(strnatcasecmp($a, $b));
}

echo "*** Testing strnatcasecmp() : variation ***\n";

str_dump('0', false);
str_dump('fooBar', '');
str_dump('', -1);
str_dump("Hello\0world", "Helloworld");
str_dump("\x0", "\0");
str_dump($a, $b);

?>
===DONE===
--EXPECT--
*** Testing strnatcasecmp() : variation ***
int(1)
int(1)
int(-1)
int(-1)
int(0)
int(0)
===DONE===
