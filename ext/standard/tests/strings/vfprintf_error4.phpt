--TEST--
Test vfprintf() function : error conditions (various conditions)
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
$file = 'vfprintf_error4.txt';
$fp = fopen( $file, "a+" );

echo "\n-- Testing vfprintf() function with other strangeties  --\n";
try {
    var_dump( vfprintf( 'foo', 'bar', array( 'baz' ) ) );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump( vfprintf( $fp, 'Foo %$c-0202Sd', array( 2 ) ) );
} catch(\ValueError $e) {
    print('Error found: '.$e->getMessage().".\n");
}
// Close handle
fclose( $fp );

?>
--CLEAN--
<?php

$file = 'vfprintf_error4.txt';
unlink( $file );

?>
--EXPECTF--
-- Testing vfprintf() function with other strangeties  --
vfprintf() expects parameter 1 to be resource, string given
Error found: Argument number must be greater than zero.
