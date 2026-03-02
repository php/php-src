--TEST--
intl_is_failure()
--EXTENSIONS--
intl
--FILE--
<?php
/*
 * Check determining failure error codes.
 */


function check( $err_code )
{
    var_export( intl_is_failure( $err_code ) );
    echo "\n";
}

check( U_ZERO_ERROR );
check( U_USING_FALLBACK_WARNING );
check( U_ILLEGAL_ARGUMENT_ERROR );
?>
--EXPECT--
false
false
true
