--TEST--
Test vfprintf() function : error conditions (more than expected arguments)
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

echo "\n-- Testing vfprintf() function with more than expected no. of arguments --\n";
$format = 'string_val';
$args = array( 1, 2 );
$extra_arg = 10;
var_dump( vfprintf( $fp, $format, $args, $extra_arg ) );
var_dump( vfprintf( $fp, "Foo %d", array(6), "bar" ) );

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
-- Testing vfprintf() function with more than expected no. of arguments --

Warning: Wrong parameter count for vfprintf() in %s on line %d
NULL

Warning: Wrong parameter count for vfprintf() in %s on line %d
NULL
===DONE===
