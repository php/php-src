--TEST--
Cloning numfmt
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
include_once( 'ut_common.inc' );
$GLOBALS['oo-mode'] = true;
$res_str = '';
/*
 * Clone
 */
$test_value = 12345.123456;
$fmt = new NumberFormatter( "en_US", NumberFormatter::PATTERN_DECIMAL );
$res_str .= "Formatted number: " . ut_nfmt_format( $fmt, $test_value ) . "\n";
$fmt_clone = clone $fmt;

$res = $fmt->setPattern("0.0" );
if( $res === false )
    $res_str .= ut_nfmt_get_error_message( $fmt ) . " (" . ut_nfmt_get_error_code( $fmt ) . ")\n";

$res_str .= "Formatted number: " . ut_nfmt_format( $fmt, $test_value ) . "\n";
$res_str .= "Formatted(clone) number: " . ut_nfmt_format( $fmt_clone, $test_value ) . "\n";
echo $res_str;

?>
--EXPECT--
Formatted number: 12345.123456
Formatted number: 12345.1
Formatted(clone) number: 12345.123456
