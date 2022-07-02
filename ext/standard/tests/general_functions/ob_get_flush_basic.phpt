--TEST--
Test ob_get_flush() function : basic functionality
--INI--
output_buffering=0
--FILE--
<?php
echo "*** Testing ob_get_flush() : basic functionality ***\n";

ob_start();

echo "testing ob_get_flush() with some\nNewlines too\n";
$string = ob_get_flush();

var_dump( "this is printed before returning the string" );
var_dump( $string );
var_dump( ob_list_handlers() );

// Empty string expected
ob_start();
$string = ob_get_flush();
var_dump($string)

?>
--EXPECT--
*** Testing ob_get_flush() : basic functionality ***
testing ob_get_flush() with some
Newlines too
string(43) "this is printed before returning the string"
string(46) "testing ob_get_flush() with some
Newlines too
"
array(0) {
}
string(0) ""
