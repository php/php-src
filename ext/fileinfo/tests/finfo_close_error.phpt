--TEST--
Test finfo_close() function : error conditions
--EXTENSIONS--
fileinfo
--FILE--
<?php
echo "*** Testing finfo_close() : error conditions ***\n";

echo "\n-- Testing finfo_close() function with wrong resource type --\n";
$fp = fopen( __FILE__, 'r' );
try {
    var_dump( finfo_close( $fp ) );
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
*** Testing finfo_close() : error conditions ***

-- Testing finfo_close() function with wrong resource type --

Deprecated: Function finfo_close() is deprecated since 8.5, as finfo objects are freed automatically in %s on line %d
finfo_close(): Argument #1 ($finfo) must be of type finfo, resource given
