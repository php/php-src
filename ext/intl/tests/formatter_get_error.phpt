--TEST--
numfmt_get_error_message/code()
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

/*
 * Error handling.
 */


function ut_main()
{
    $fmt = ut_nfmt_create( "en_US", NumberFormatter::CURRENCY );
    $currency = '';
    $pos = 0;
    $num = ut_nfmt_parse_currency( $fmt, '123.45', $currency, $pos );
    if( $num === false )
        return $fmt->getErrorMessage() . " (" . $fmt->getErrorCode() . ")\n";
    else
        return "Ooops, an error should have occurred.";
}

include_once( 'ut_common.inc' );

// Run the test
ut_run();
?>
--EXPECT--
Number parsing failed: U_PARSE_ERROR (9)
