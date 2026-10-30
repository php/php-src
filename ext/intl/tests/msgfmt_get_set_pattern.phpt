--TEST--
msgfmt_get/set_pattern()
--EXTENSIONS--
intl
--FILE--
<?php

/*
 * Get/set pattern.
 */


function ut_main()
{
    $res_str = '';
    $fmt = ut_msgfmt_create( "en_US", "{0,number} monkeys on {1,number} trees" );

    // Get default patten.
    $res_str .= "Default pattern: '" . ut_msgfmt_get_pattern( $fmt ) . "'\n";
    $res_str .= "Formatting result: " . ut_msgfmt_format( $fmt, array(123, 456) ) . "\n";

    // Set a new pattern.
    $pattern = "{0,number} trees hosting {1,number} monkeys";
    $res = ut_msgfmt_set_pattern( $fmt, $pattern );
    if( $res === false )
        $res_str .= ut_msgfmt_get_error_message( $fmt ) . " (" . ut_msgfmt_get_error_code( $fmt ) . ")\n";

    // Check if the pattern has been changed.
    $res = ut_msgfmt_get_pattern( $fmt );
    if( $res === false )
        $res_str .= ut_msgfmt_get_error_message( $fmt ) . " (" . ut_msgfmt_get_error_code( $fmt ) . ")\n";
    $res_str .= "New pattern: '" . ut_msgfmt_get_pattern( $fmt ) . "'\n";
    $res_str .= "Formatted message: " . ut_msgfmt_format( $fmt, array(123, 456) ) . "\n";

    ut_msgfmt_set_pattern($fmt, str_repeat($pattern, 10));
    $res_str .= "New pattern: '" . ut_msgfmt_get_pattern( $fmt ) . "'\n";
    $res_str .= "Formatted message: " . ut_msgfmt_format( $fmt, array(123, 456) ) . "\n";
    $res = ut_msgfmt_set_pattern($fmt, "{0,number} trees hosting {1,number monkeys");
    if ($res !== false) die("ut_msgfmt_set_pattern should fail");
    $res_str .= ut_msgfmt_get_error_message( $fmt ) . " (" . ut_msgfmt_get_error_code( $fmt ) . ")\n";


    return $res_str;
}

include_once( 'ut_common.inc' );
ut_run();

?>
--EXPECT--
Default pattern: '{0,number} monkeys on {1,number} trees'
Formatting result: 123 monkeys on 456 trees
New pattern: '{0,number} trees hosting {1,number} monkeys'
Formatted message: 123 trees hosting 456 monkeys
New pattern: '{0,number} trees hosting {1,number} monkeys{0,number} trees hosting {1,number} monkeys{0,number} trees hosting {1,number} monkeys{0,number} trees hosting {1,number} monkeys{0,number} trees hosting {1,number} monkeys{0,number} trees hosting {1,number} monkeys{0,number} trees hosting {1,number} monkeys{0,number} trees hosting {1,number} monkeys{0,number} trees hosting {1,number} monkeys{0,number} trees hosting {1,number} monkeys'
Formatted message: 123 trees hosting 456 monkeys123 trees hosting 456 monkeys123 trees hosting 456 monkeys123 trees hosting 456 monkeys123 trees hosting 456 monkeys123 trees hosting 456 monkeys123 trees hosting 456 monkeys123 trees hosting 456 monkeys123 trees hosting 456 monkeys123 trees hosting 456 monkeys
Error setting symbol value at line 0, offset 26: U_PATTERN_SYNTAX_ERROR (65799)
