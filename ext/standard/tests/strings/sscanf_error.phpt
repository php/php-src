--TEST--
Test sscanf() function : error conditions
--FILE--
<?php

echo "*** Testing sscanf() : error conditions ***\n";

$str = "Hello World";
$format = "%s %s";

echo "\n-- Testing sscanf() function with more than expected no. of arguments --\n";

try {
    sscanf($str, $format, $str1, $str2, $extra_str);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
*** Testing sscanf() : error conditions ***

-- Testing sscanf() function with more than expected no. of arguments --
Variable is not assigned by any conversion specifiers
