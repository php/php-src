--TEST--
Test vfprintf() function : variation functionality
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>
--INI--
precision=14
--FILE--
<?php
echo "*** Testing vfprintf() : variation functionality ***\n";

// Open handle
$file = 'vfprintf_variation1.txt';
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
    try {
        ftruncate( $fp, 0 );
        $length = vfprintf( $fp, $format, $args );
        rewind( $fp );
        $content = stream_get_contents( $fp );
        var_dump( $content );
        var_dump( $length );
    } catch (TypeError $exception) {
        echo $exception->getMessage() . "\n";
    }
}

// Test vfprintf()
writeAndDump( $fp, "format", null );
writeAndDump( $fp, "Foo is %d and %s", array( 30, 'bar' ) );
writeAndDump( $fp, "Foobar testing", array() );
writeAndDump( $fp, "%s %s %s", array( 'bar', 'bar', 'bar' ) );
writeAndDump( $fp, "%02d", array( 50 ) );
writeAndDump( $fp, "", array() );
writeAndDump( $fp, "Testing %b %d %f %o %s %x %X", array( 9, 6, 2.5502, 24, "foobar", 15, 65 ) );

// Close handle
fclose( $fp );

?>
--CLEAN--
<?php

$file = 'vfprintf_variation1.txt';
unlink( $file );

?>
--EXPECT--
*** Testing vfprintf() : variation functionality ***
vfprintf(): Argument #3 ($values) must be of type array, null given
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
