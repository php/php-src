--TEST--
Test vfprintf() function : variation functionality
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

echo "*** Testing vfprintf() : variation functionality ***\n";

// Open handle
$file = 'vfprintf_test.txt';
$fp = fopen( $file, 'a+' );

$funset = fopen( __FILE__, 'r' );
unset( $funset );

class FooClass
{
	public function __toString()
	{
		return "Object";
	}
}

// Output facilitating function
function writeAndDump($fp, $format, $args)
{
	ftruncate( $fp, 0 );
	$length = vfprintf( $fp, $format, $args );
	rewind( $fp );
	$content = stream_get_contents( $fp );
	var_dump( $content );
	var_dump( $length );
}

// Test vfprintf()
writeAndDump( $fp, "format", null );
writeAndDump( $fp, "Foo is %d and %s", array( 30, 'bar' ) );
writeAndDump( $fp, "Foobar testing", array() );
writeAndDump( $fp, "%s %s %s", array( 'bar', 'bar', 'bar' ) );
writeAndDump( $fp, "%02d", array( 50 ) );
writeAndDump( $fp, "", array() );
writeAndDump( $fp, "Testing %b %d %f %o %s %x %X", array( 9, 6, 2.5502, 24, "foobar", 15, 65 ) );
@writeAndDump( $funset, "Foo with %s", array( 'string' ) );
@writeAndDump( new FooClass(), "Foo with %s", array( 'string' ) );

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
*** Testing vfprintf() : variation functionality ***
string(6) "format"
int(6)
string(17) "Foo is 30 and bar"
int(17)
string(14) "Foobar testing"
int(14)
string(11) "bar bar bar"
int(11)
string(2) "50"
int(2)
string(0) ""
int(0)
string(38) "Testing 1001 6 2.550200 30 foobar f 41"
int(38)
bool(false)
bool(false)
bool(false)
bool(false)
===DONE===
