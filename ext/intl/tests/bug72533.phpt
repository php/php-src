--TEST--
Bug #72533 (locale_accept_from_http out-of-bounds access)
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php

function ut_main()
{
    $ret = var_export(ut_loc_accept_http(str_repeat('x', 256)), true);
    $ret .= "\n";
	if(intl_is_failure(intl_get_error_code())) {
		$ret .= var_export(intl_get_error_message(), true);
	}
    $ret .= "\n";
    $ret .= var_export(ut_loc_accept_http(str_repeat('en,', 256)), true);
    $ret .= "\n";
	if(intl_is_failure(intl_get_error_code())) {
		$ret .= var_export(intl_get_error_message(), true);
	}
    return $ret;
}

include_once( 'ut_common.inc' );
ut_run();
?>
--EXPECT--
false
'locale_accept_from_http: locale string too long: U_ILLEGAL_ARGUMENT_ERROR'
'en'
