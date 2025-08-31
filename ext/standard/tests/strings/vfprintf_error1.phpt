--TEST--
Test vfprintf() function : error conditions (more than expected arguments)
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>
--INI--
precision=14
--FILE--
<?php
// Open handle
$file = 'vfprintf_error1.txt';
$fp = fopen( $file, "a+" );

echo "\n-- Testing vfprintf() function with more than expected no. of arguments --\n";
$format = 'string_val';
$args = array( 1, 2 );
$extra_arg = 10;
try {
    var_dump( vfprintf( $fp, $format, $args, $extra_arg ) );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump( vfprintf( $fp, "Foo %d", array(6), "bar" ) );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

// Close handle
fclose($fp);

?>
--CLEAN--
<?php

$file = 'vfprintf_error1.txt';
unlink( $file );

?>
--EXPECT--
-- Testing vfprintf() function with more than expected no. of arguments --
vfprintf() expects exactly 3 arguments, 4 given
vfprintf() expects exactly 3 arguments, 4 given
