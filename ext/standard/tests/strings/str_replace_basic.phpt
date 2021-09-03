--TEST--
Test str_replace() function basic function
--INI--
precision=14
--FILE--
<?php
echo "\n*** Testing str_replace() on basic operations ***\n";

var_dump( str_replace("", "", "") );

var_dump( str_replace("e", "b", "test") );

var_dump( str_replace("", "", "", $count) );
var_dump( $count );

var_dump( str_replace("q", "q", "q", $count) );
var_dump( $count );

var_dump( str_replace("long string here", "", "", $count) );
var_dump( $count );

$fp = fopen( __FILE__, "r" );
$fp_copy = $fp;
try {
    var_dump( str_replace($fp_copy, $fp_copy, $fp_copy, $fp_copy) );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump( $fp_copy );
fclose($fp);

?>
--EXPECTF--
*** Testing str_replace() on basic operations ***
string(0) ""
string(4) "tbst"
string(0) ""
int(0)
string(1) "q"
int(1)
string(0) ""
int(0)
str_replace(): Argument #1 ($search) must be of type array|string, resource given
resource(%d) of type (stream)
