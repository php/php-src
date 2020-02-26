--TEST--
Test str_pad() function : usage variations - large values for '$pad_length' argument
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") {
    die("skip Zend MM disabled");
}
?>
--FILE--
<?php
/* Prototype  : string str_pad  ( string $input  , int $pad_length  [, string $pad_string  [, int $pad_type  ]] )
 * Description: Pad a string to a certain length with another string
 * Source code: ext/standard/string.c
*/

/* Test str_pad() function: with unexpected inputs for '$pad_length'
 *  and expected type for '$input'
*/

echo "*** Testing str_pad() function: with large value for for 'pad_length' argument ***\n";

//defining '$input' argument
$input = "Test string";

$extra_large_pad_length = PHP_INT_MAX*5;
try {
    var_dump( str_pad($input, $extra_large_pad_length) );
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}

$php_int_max_pad_length = PHP_INT_MAX;
var_dump( str_pad($input, $php_int_max_pad_length) );


?>
--EXPECTF--
*** Testing str_pad() function: with large value for for 'pad_length' argument ***
str_pad(): Argument #2 ($pad_length) must be of type int, float given

Fatal error: Allowed memory size of %d bytes exhausted%s(tried to allocate %d bytes) in %s on line %d
