--TEST--
Test mb_substitute_character() function : basic functionality 
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

echo "*** Testing mb_substitute_character() : basic functionality ***\n";


// Initialise all required variables
var_dump( mb_substitute_character() );
var_dump( mb_substitute_character(66) );
var_dump( mb_substitute_character() );
var_dump( mb_substitute_character(1234) );
var_dump( mb_substitute_character() );
var_dump( mb_substitute_character("none") );
var_dump( mb_substitute_character() );
var_dump( mb_substitute_character("b") );

?>
===DONE===
--EXPECTF--
*** Testing mb_substitute_character() : basic functionality ***
int(63)
bool(true)
int(66)
bool(true)
int(1234)
bool(true)
string(4) "none"

Warning: mb_substitute_character(): Unknown character. in %s on line %d
bool(false)
===DONE===
