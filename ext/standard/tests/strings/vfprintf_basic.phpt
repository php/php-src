--TEST--
Test vfprintf() function : basic functionality
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

function writeAndDump($fp, $format, $args)
{
	ftruncate( $fp, 0 );
	$length = vfprintf( $fp, $format, $args );
	rewind( $fp );
	$content = stream_get_contents( $fp );
	var_dump( $content );
	var_dump( $length );
}

echo "*** Testing vfprintf() : basic functionality ***\n";

// Open handle
$file = 'vfprintf_test.txt';
$fp = fopen( $file, "a+" );

// Test vfprintf()
writeAndDump( $fp, "Foo is %d and %s", array( 30, 'bar' ) );
writeAndDump( $fp, "%s %s %s", array( 'bar', 'bar', 'bar' ) );
writeAndDump( $fp, "%d digit", array( '54' ) );
writeAndDump( $fp, "%b %b", array( true, false ) );
writeAndDump( $fp, "%c %c %c", array( 65, 66, 67 ) );
writeAndDump( $fp, "%e %E %e", array( 1000, 2e4, +2e2 ) );
writeAndDump( $fp, "%02d", array( 50 ) );
writeAndDump( $fp, "Testing %b %d %f %s %x %X", array( 9, 6, 2.5502, "foobar", 15, 65 ) );

// Close handle
fclose( $fp );

?>
===DONE===
--CLEAN--
<?php

$file = 'vfprintf_test.txt';
unlink( $file );

?>
--EXPECT--
*** Testing vfprintf() : basic functionality ***
string(17) "Foo is 30 and bar"
int(17)
string(11) "bar bar bar"
int(11)
string(8) "54 digit"
int(8)
string(3) "1 0"
int(3)
string(5) "A B C"
int(5)
string(35) "1.000000e+3 2.000000E+4 2.000000e+2"
int(35)
string(2) "50"
int(2)
string(35) "Testing 1001 6 2.550200 foobar f 41"
int(35)
===DONE===
