--TEST--
numfmt_parse()
--EXTENSIONS--
intl
--FILE--
<?php

/*
 * Number parsing.
 */


function ut_main()
{
    $res_str = '';

    // Test parsing float number.
    $fmt = ut_nfmt_create( "en_US", NumberFormatter::DECIMAL );
    $res_str .= ut_nfmt_parse( $fmt, "123E-3" ) . "\n";

    // Test parsing float number as integer.
    $fmt = ut_nfmt_create( "en_US", NumberFormatter::DECIMAL );
    $res_str .= ut_nfmt_parse( $fmt, "1.23", NumberFormatter::TYPE_INT32 ) . "\n";

    // Test specifying non-zero parsing start position.
    $fmt = ut_nfmt_create( "en_US", NumberFormatter::DECIMAL );
    $pos = 2;
    $res_str .= ut_nfmt_parse( $fmt, "0.123 here", NumberFormatter::TYPE_DOUBLE, $pos ) . "\n";
    $res_str .= "$pos\n";

    return $res_str;
}

include_once( 'ut_common.inc' );
ut_run();

?>
--EXPECT--
0.123
1
123
5
