--TEST--
Test str_pad() function : usage variations - unexpected large value for '$pad_length' argument
--INI--
memory_limit=128M
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
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
$pad_length = PHP_INT_MAX - 16; /* zend_string header is 16 bytes */
var_dump( str_pad($input, $pad_length) );

?>
--EXPECTF--
*** Testing str_pad() function: with large value for for 'pad_length' argument ***

Fatal error: Allowed memory size of 134217728 bytes exhausted%s(tried to allocate %d bytes) in %s on line %d
