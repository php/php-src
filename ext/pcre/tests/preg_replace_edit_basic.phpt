--TEST--
Test preg_replace() function : basic 
--FILE--
<?php
/* Prototype  : proto string preg_replace(mixed regex, mixed replace, mixed subject [, int limit [, count]])
 * Description: Perform Perl-style regular expression replacement. 
 * Source code: ext/pcre/php_pcre.c
 * Alias to functions: 
*/

$string = '123456789 - Hello, world -           This is a string.';
var_dump($string);

var_dump(preg_replace('<- This is a string$>',
                      'This shouldn\'t work', $string));				//tries to find '- This is a string' at the end of a string but can't so replaces nothing and prints the unchanged $string.

var_dump(preg_replace('<[0-35-9]>', 
                      '4', $string,               					//finds any number that's not 4 and replaces it with a 4
			    '5', $count));							//limits to 5 replacements returns 444444789
var_dump($count);											//counts the number of replacements made (5)


var_dump(preg_replace('<\b[hH]\w{2,4}>',
                      'Bonjour', $string));						//finds h or H at the beginning of a word followed by 2-4 characters and replaces it with Bonjour (i.e. Hello -> Bonjour) (was finding the 'his' in This and replacing it)

var_dump(preg_replace('<(\w)\s*-\s*(\w)>',
                      '\\1. \\2', $string));						//finds dashes with an indefinate amount of whitespace around them and replaces them with a full stop precedeby no spaces and followed by one space

var_dump(preg_replace('<(^[a-z]\w+)@(\w+)\.(\w+)\.([a-z]{2,}$)>', 
                      '\\1 at \\2 dot \\3 dot \\4', 'josmessa@uk.ibm.com'));	//finds the e-mail address and replaces the @ and . with "at" and "dot" (uses backreferences) ('josmessa at uk dot ibm dot com')
?>
--EXPECTF--
string(54) "123456789 - Hello, world -           This is a string."
string(54) "123456789 - Hello, world -           This is a string."
string(54) "444444789 - Hello, world -           This is a string."
int(5)
string(56) "123456789 - Bonjour, world -           This is a string."
string(42) "123456789. Hello, world. This is a string."
string(30) "josmessa at uk dot ibm dot com"
