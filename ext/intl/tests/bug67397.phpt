--TEST--
Bug #67397 (Buffer overflow in locale_get_display_name->uloc_getDisplayName (libicu 4.8.1))
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

function ut_main()
{
    $ret = var_export(ut_loc_get_display_name(str_repeat('*', 256), 'en_us'), true);
    $ret .= "\n";
    $ret .= var_export(intl_get_error_message(), true);
    return $ret;
}

include_once( 'ut_common.inc' );
ut_run();
?>
--EXPECT--
false
'locale_get_display_name : name too long: U_ILLEGAL_ARGUMENT_ERROR'
