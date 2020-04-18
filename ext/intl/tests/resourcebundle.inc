<?php
// FIXME Improve Endian check when endian const gets added
// Check if platform is Big or Little endian
if (pack('S', 0xABCD) !== pack('v', 0xABCD)) {
    $endian = 'be';
} else {
    $endian = 'le';
}

define('BUNDLE', __DIR__ . '/_files/resourcebundle_' . $endian);

function debug( $res ) {
    if (is_null( $res )) {
        $ret = "NULL\n";
    }
    else {
        $ret = print_r( $res, true ). "\n";
    }
    return $ret . sprintf( "%5d: %s\n", intl_get_error_code(), intl_get_error_message() );
}
