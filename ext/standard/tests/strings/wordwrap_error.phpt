--TEST--
Test wordwrap() function : error conditions 
--FILE--
<?php
/* Prototype  : string wordwrap ( string $str [, int $width [, string $break [, bool $cut]]] )
 * Description: Wraps buffer to selected number of characters using string break char
 * Source code: ext/standard/string.c
*/

echo "*** Testing wordwrap() : error conditions ***\n";

// Zero argument
echo "\n-- Testing wordwrap() function with Zero arguments --\n";
var_dump( wordwrap() );

// More than expected number of arguments
echo "\n-- Testing wordwrap() function with more than expected no. of arguments --\n";
$str = 'testing wordwrap function';
$width = 10;
$break = '<br />\n';
$cut = true;
$extra_arg = "extra_arg";

var_dump( wordwrap($str, $width, $break, $cut, $extra_arg) );

// $width arg as negative value
echo "\n-- Testing wordwrap() function with negative/zero value for width argument --\n";
echo "-- width = 0 & cut = false --\n";
// width as zero and cut as false
$width = 0;
$cut = false;
var_dump( wordwrap($str, $width, $break, $cut) );

echo "-- width = 0 & cut = true --\n";
// width as zero and cut as true 
$width = 0;
$cut = true;
var_dump( wordwrap($str, $width, $break, $cut) );

echo "-- width = -10 & cut = false --\n";
// width as -ne and cut as false
$width = -10;
$cut = false;
var_dump( wordwrap($str, $width, $break, $cut) );

echo "-- width = -10 & cut = true --\n";
// width as -ne and cut as true 
$width = -10;
$cut = true;
var_dump( wordwrap($str, $width, $break, $cut) );

echo "Done\n";
?>
--EXPECTF--
*** Testing wordwrap() : error conditions ***

-- Testing wordwrap() function with Zero arguments --

Warning: wordwrap() expects at least 1 parameter, 0 given in %s on line %d
NULL

-- Testing wordwrap() function with more than expected no. of arguments --

Warning: wordwrap() expects at most 4 parameters, 5 given in %s on line %d
NULL

-- Testing wordwrap() function with negative/zero value for width argument --
-- width = 0 & cut = false --
string(39) "testing<br />\nwordwrap<br />\nfunction"
-- width = 0 & cut = true --

Warning: wordwrap(): Can't force cut when width is zero in %s on line %d
bool(false)
-- width = -10 & cut = false --
string(39) "testing<br />\nwordwrap<br />\nfunction"
-- width = -10 & cut = true --
string(223) "<br />\nt<br />\ne<br />\ns<br />\nt<br />\ni<br />\nn<br />\ng<br />\n<br />\nw<br />\no<br />\nr<br />\nd<br />\nw<br />\nr<br />\na<br />\np<br />\n<br />\nf<br />\nu<br />\nn<br />\nc<br />\nt<br />\ni<br />\no<br />\nn"
Done
