--TEST--
Test wordwrap() function : error conditions
--FILE--
<?php
/* Prototype  : string wordwrap ( string $str [, int $width [, string $break [, bool $cut]]] )
 * Description: Wraps buffer to selected number of characters using string break char
 * Source code: ext/standard/string.c
*/

echo "*** Testing wordwrap() : error conditions ***\n";

$str = 'testing wordwrap function';
$width = 10;
$break = '<br />\n';
$cut = true;

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

try {
    wordwrap($str, $width, $break, $cut);
} catch (\Error $e) {
    echo $e->getMessage() . "\n";
}

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

-- Testing wordwrap() function with negative/zero value for width argument --
-- width = 0 & cut = false --
string(39) "testing<br />\nwordwrap<br />\nfunction"
-- width = 0 & cut = true --
Can't force cut when width is zero
-- width = -10 & cut = false --
string(39) "testing<br />\nwordwrap<br />\nfunction"
-- width = -10 & cut = true --
string(223) "<br />\nt<br />\ne<br />\ns<br />\nt<br />\ni<br />\nn<br />\ng<br />\n<br />\nw<br />\no<br />\nr<br />\nd<br />\nw<br />\nr<br />\na<br />\np<br />\n<br />\nf<br />\nu<br />\nn<br />\nc<br />\nt<br />\ni<br />\no<br />\nn"
Done
