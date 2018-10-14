--TEST--
Test wordwrap() function : basic functionality
--FILE--
<?php
/* Prototype  : string wordwrap ( string $str [, int $width [, string $break [, bool $cut]]] )
 * Description: Wraps buffer to selected number of characters using string break char
 * Source code: ext/standard/string.c
*/

echo "*** Testing wordwrap() : basic functionality ***\n";

// Initialize all required variables
$str = "The quick brown foooooooooox jummmmmmmmmmmmped over the lazzzzzzzzzzzy doooooooooooooooooooooog.";
$width = 80;
$break = '<br />\n';

// Calling wordwrap() with default arguments
var_dump( wordwrap($str) );

// Calling wordwrap() with all possible optional arguments
// with $width arg
var_dump( wordwrap($str, $width) );
// with $break arg
var_dump( wordwrap($str, $width, $break) );

// Calling wordwrap() with all arguments
// $cut as true
$width = 10;
$cut = true;
var_dump( wordwrap($str, $width, $break, $cut) );

// $cut as false
$width = 10;
$cut = false;
var_dump( wordwrap($str, $width, $break, $cut) );
echo "Done\n";
?>
--EXPECTF--
*** Testing wordwrap() : basic functionality ***
string(96) "The quick brown foooooooooox jummmmmmmmmmmmped over the lazzzzzzzzzzzy
doooooooooooooooooooooog."
string(96) "The quick brown foooooooooox jummmmmmmmmmmmped over the lazzzzzzzzzzzy
doooooooooooooooooooooog."
string(103) "The quick brown foooooooooox jummmmmmmmmmmmped over the lazzzzzzzzzzzy<br />\ndoooooooooooooooooooooog."
string(178) "The quick<br />\nbrown<br />\nfooooooooo<br />\nox<br />\njummmmmmmm<br />\nmmmmped<br />\nover the<br />\nlazzzzzzzz<br />\nzzzy<br />\ndooooooooo<br />\noooooooooo<br />\nooog."
string(138) "The quick<br />\nbrown<br />\nfoooooooooox<br />\njummmmmmmmmmmmped<br />\nover the<br />\nlazzzzzzzzzzzy<br />\ndoooooooooooooooooooooog."
Done
