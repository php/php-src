--TEST--
numfmt_parse_currency()
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

/*
 * Currency parsing.
 */

function ut_main()
{
    $res_str = '';

    $fmt = ut_nfmt_create( "en_US", NumberFormatter::CURRENCY );
    $pos = 0;
    $currency = '';
    $num = ut_nfmt_parse_currency( $fmt, '$9,988,776.65', $currency, $pos );
    $res_str .= "$num $currency\n";

    $fmt = ut_nfmt_create( "en_US", NumberFormatter::CURRENCY );
    $pos = 1;
    $currency = '';
    $num = ut_nfmt_parse_currency( $fmt, ' $123.45', $currency, $pos );
    $res_str .=  "$num $currency\n";

    return $res_str;
}

include_once( 'ut_common.inc' );
ut_run();

?>
--EXPECT--
9988776.65 USD
123.45 USD
