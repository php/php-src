--TEST--
intl_get_error_message()
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
/*
 * Check getting global error message.
 */

// Suppress warning messages.
error_reporting( E_ERROR );

if( collator_get_locale(new Collator('en_US'), -1) !== false )
    echo "failed\n";
else
    printf( "%s\n", intl_get_error_message() );

?>
--EXPECT--
Error getting locale by type: U_ILLEGAL_ARGUMENT_ERROR
