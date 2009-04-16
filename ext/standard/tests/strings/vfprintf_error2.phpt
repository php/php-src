--TEST--
Test vfprintf() function : error conditions (less than expected arguments) 
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>
--INI--
precision=14
--FILE--
<?php
/* Prototype  : int vfprintf(resource stream, string format, array args)
 * Description: Output a formatted string into a stream 
 * Source code: ext/standard/formatted_print.c
 * Alias to functions: 
 */

// Open handle
$file = 'vfprintf_test.txt';
$fp = fopen( $file, "a+" );

echo "\n-- Testing vfprintf() function with less than expected no. of arguments --\n";
$format = 'string_val';
var_dump( vfprintf($fp, $format) );
var_dump( vfprintf( $fp ) );
var_dump( vfprintf() );

// Close handle
fclose($fp);

?>
===DONE===
--CLEAN--
<?php

$file = 'vfprintf_test.txt';
unlink( $file );

?>
--EXPECTF--
-- Testing vfprintf() function with less than expected no. of arguments --

Warning: Wrong parameter count for vfprintf() in %s on line %d
NULL

Warning: Wrong parameter count for vfprintf() in %s on line %d
NULL

Warning: Wrong parameter count for vfprintf() in %s on line %d
NULL
===DONE===
