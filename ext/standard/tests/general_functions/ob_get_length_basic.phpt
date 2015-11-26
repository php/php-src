--TEST--
Test ob_get_length() function : basic functionality 
--INI--
output_buffering=0
--FILE--
<?php
/* Prototype  : int ob_get_length(void)
 * Description: Return the length of the output buffer 
 * Source code: main/output.c
 * Alias to functions: 
 */

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

// Extra argument
var_dump( ob_get_length( 'foobar' ) );

?>
===DONE===
--EXPECTF--
*** Testing ob_get_length() : basic functionality ***
bool(false)
int(26)
int(8)
int(1)
int(15)
int(0)
int(0)

Warning: ob_get_length() expects exactly 0 parameters, 1 given in %s on line %d
NULL
===DONE===
