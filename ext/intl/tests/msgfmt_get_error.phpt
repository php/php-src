--TEST--
msgmfmt_get_error_message/code()
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

/*
 * Error handling.
 */


function ut_main()
{
    $fmt = ut_msgfmt_create( "en_US", "{0, number} monkeys on {1, number} trees" );
    $num = ut_msgfmt_format( $fmt, array());
    if( $num === false )
        return $fmt->getErrorMessage() . " (" . $fmt->getErrorCode() . ")\n";
    else
        return "Ooops, an error should have occured.";
}

include_once( 'ut_common.inc' );

// Run the test
ut_run();
?>
--EXPECT--
msgfmt_format: not enough parameters: U_ILLEGAL_ARGUMENT_ERROR (1)
