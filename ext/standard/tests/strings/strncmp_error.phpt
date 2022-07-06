--TEST--
Test strncmp() function : error conditions
--FILE--
<?php
/* Test strncmp() function with more/less number of args and invalid args */

echo "*** Testing strncmp() function: error conditions ***\n";
$str1 = 'string_val';
$str2 = 'string_val';

/* Invalid argument for $len */
$len = -10;

try {
    var_dump( strncmp($str1, $str2, $len) );
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing strncmp() function: error conditions ***
strncmp(): Argument #3 ($length) must be greater than or equal to 0
