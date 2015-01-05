--TEST--
intl_get_error_code()
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
/*
 * Check getting global error code.
 */

// Suppress warning messages.
error_reporting( E_ERROR );

if( collator_get_locale() !== false )
    echo "failed\n";
else
{
    $check_code = ( intl_get_error_code() != 0 );
    echo ( $check_code ?  "ok" : "failed" ) . "\n";
}

?>
--EXPECT--
ok
