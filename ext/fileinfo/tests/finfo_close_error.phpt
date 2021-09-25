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
--EXPECT--
*** Testing finfo_close() : error conditions ***

-- Testing finfo_close() function with wrong resource type --
finfo_close(): Argument #1 ($finfo) must be of type finfo, resource given
