--TEST--
Test strpbrk() function : basic functionality
--FILE--
<?php
echo "*** Testing strpbrk() : basic functionality ***\n";

// Initialise all required variables
$text = 'This is a Simple text.';
var_dump( strpbrk($text, 'mi') );
var_dump( strpbrk($text, 'ZS') );
var_dump( strpbrk($text, 'Z') );
var_dump( strpbrk($text, 'H') );

$text = '';
var_dump( strpbrk($text, 'foo') );

$text = "  aaa aaaSLR";
var_dump( strpbrk($text, '     ') );

var_dump( strpbrk(5, 5) );
var_dump( strpbrk(5, "5") );

?>
--EXPECT--
*** Testing strpbrk() : basic functionality ***
string(20) "is is a Simple text."
string(12) "Simple text."
bool(false)
bool(false)
bool(false)
string(12) "  aaa aaaSLR"
string(1) "5"
string(1) "5"
