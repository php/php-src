--TEST--
Test mb_substitute_character() function : error conditions
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_substitute_character') or die("skip mb_substitute_character() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : mixed mb_substitute_character([mixed substchar])
 * Description: Sets the current substitute_character or returns the current substitute_character
 * Source code: ext/mbstring/mbstring.c
 * Alias to functions:
 */

echo "*** Testing mb_substitute_character() : error conditions ***\n";


//Test mb_substitute_character with one more than the expected number of arguments
echo "\n-- Testing mb_substitute_character() function with more than expected no. of arguments --\n";
$substchar = 1;
$extra_arg = 10;
var_dump( mb_substitute_character($substchar, $extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing mb_substitute_character() : error conditions ***

-- Testing mb_substitute_character() function with more than expected no. of arguments --

Warning: mb_substitute_character() expects at most 1 parameter, 2 given in %s on line %d
NULL
===DONE===
