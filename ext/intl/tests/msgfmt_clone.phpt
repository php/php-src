--TEST--
Cloning msgfmt
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
$fmt = ut_msgfmt_create( "en_US", "{0,number} monkeys on {1,number} trees" );

// Get default patten.
$res_str .= "Formatting result: " . ut_msgfmt_format( $fmt, array(123, 456) ) . "\n";
$fmt_clone = clone $fmt;
// Set a new pattern.
$pattern = "{0,number} trees hosting {1,number} monkeys";
$res = ut_msgfmt_set_pattern( $fmt, $pattern );
$res_str .= "Formatting result: " . ut_msgfmt_format( $fmt, array(123, 456) ) . "\n";
$res_str .= "Formatting clone result: " . ut_msgfmt_format( $fmt_clone, array(123, 456) ) . "\n";

echo $res_str;

?>
--EXPECTF--
Formatting result: 123 monkeys on 456 trees
Formatting result: 123 trees hosting 456 monkeys
Formatting clone result: 123 monkeys on 456 trees
