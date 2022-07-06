--TEST--
intl_error_name()
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

/*
 * Check getting error string by integer error code.
 */


function check( $err_code )
{
    echo intl_error_name( $err_code ) . "\n";
}

check( U_ZERO_ERROR );
check( U_ILLEGAL_ARGUMENT_ERROR );
check( U_USING_FALLBACK_WARNING );
?>
--EXPECT--
U_ZERO_ERROR
U_ILLEGAL_ARGUMENT_ERROR
U_USING_FALLBACK_WARNING
