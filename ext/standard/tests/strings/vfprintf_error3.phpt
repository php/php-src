--TEST--
Test vfprintf() function : error conditions (wrong argument types)
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>
--INI--
precision=14
--FILE--
<?php
// Open handle
$file = 'vfprintf_error3.txt';
$fp = fopen( $file, "a+" );

echo "\n-- Testing vfprintf() function with wrong variable types as argument --\n";
try {
  vfprintf($fp, array( 'foo %d', 'bar %s' ), 3.55552);
} catch (TypeError $exception) {
  echo $exception->getMessage() . "\n";
}

try {
    vfprintf($fp, "Foo: %s", "not available");
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    vfprintf($fp, "Foo %y fake", ["not available"]);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

rewind( $fp );
var_dump( stream_get_contents( $fp ) );
ftruncate( $fp, 0 );
rewind( $fp );

// Close handle
fclose( $fp );

?>
--CLEAN--
<?php

$file = 'vfprintf_error3.txt';
unlink( $file );

?>
--EXPECT--
-- Testing vfprintf() function with wrong variable types as argument --
vfprintf(): Argument #2 ($format) must be of type string, array given
vfprintf(): Argument #3 ($values) must be of type array, string given
Unknown format specifier "y"
string(0) ""
