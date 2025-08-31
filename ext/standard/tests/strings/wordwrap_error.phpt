--TEST--
Test wordwrap() function : error conditions
--FILE--
<?php
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
} catch (\ValueError $e) {
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
?>
--EXPECT--
*** Testing wordwrap() : error conditions ***

-- Testing wordwrap() function with negative/zero value for width argument --
-- width = 0 & cut = false --
string(39) "testing<br />\nwordwrap<br />\nfunction"
-- width = 0 & cut = true --
wordwrap(): Argument #4 ($cut_long_words) cannot be true when argument #2 ($width) is 0
-- width = -10 & cut = false --
string(39) "testing<br />\nwordwrap<br />\nfunction"
-- width = -10 & cut = true --
string(223) "<br />\nt<br />\ne<br />\ns<br />\nt<br />\ni<br />\nn<br />\ng<br />\n<br />\nw<br />\no<br />\nr<br />\nd<br />\nw<br />\nr<br />\na<br />\np<br />\n<br />\nf<br />\nu<br />\nn<br />\nc<br />\nt<br />\ni<br />\no<br />\nn"
