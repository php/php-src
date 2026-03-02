--TEST--
Test ob_get_length() function : basic functionality
--INI--
output_buffering=0
--FILE--
<?php
function dump_string_length( $string )
{
    ob_start();
    echo $string;
    $len = ob_get_length();
    ob_end_clean();
    var_dump( $len );
}

echo "*** Testing ob_get_length() : basic functionality ***\n";

// No buffering active
var_dump( ob_get_length() );

dump_string_length( 'foo bar length of a string' );
dump_string_length( 'plus one' );
dump_string_length( "\0" );
dump_string_length( '            lsf' );
dump_string_length( '' );
dump_string_length( null );

?>
--EXPECT--
*** Testing ob_get_length() : basic functionality ***
bool(false)
int(26)
int(8)
int(1)
int(15)
int(0)
int(0)
