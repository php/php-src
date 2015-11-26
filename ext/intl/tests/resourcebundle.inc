<?php
define('BUNDLE', dirname(__FILE__)."/_files/resourcebundle");

function debug( $res ) {
	if (is_null( $res )) {
		$ret = "NULL\n";
	}
	else {
		$ret = print_r( $res, true ). "\n";
	}
	return $ret . sprintf( "%5d: %s\n", intl_get_error_code(), intl_get_error_message() );
}

