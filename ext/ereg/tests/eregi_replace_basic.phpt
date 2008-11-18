--TEST--
Test eregi_replace() function : basic functionality - confirm case insensitivity
--FILE--

<?php
/* Prototype  : proto string eregi_replace(string pattern, string replacement, string string)
 * Description: Case insensitive replace regular expression 
 * Source code: ext/standard/reg.c
 * Alias to functions: 
 */

/*
 * Test basic functionality of eregi_replace()
 */

echo "*** Testing eregi_replace() : basic functionality ***\n";

$string = 'UPPERCASE WORDS, lowercase words, MIxED CaSe woRdS';

echo "String Before...\n";
var_dump($string);
echo "\nString after...\n";

var_dump(eregi_replace('([[:lower:]]+) word', '\\1_character', $string));

echo "Done";
?>

--EXPECTF--
*** Testing eregi_replace() : basic functionality ***
String Before...
string(50) "UPPERCASE WORDS, lowercase words, MIxED CaSe woRdS"

String after...

Deprecated: Function eregi_replace() is deprecated in %s on line %d
string(65) "UPPERCASE_characterS, lowercase_characters, MIxED CaSe_characterS"
Done
