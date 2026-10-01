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
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}
?>
--EXPECT--
*** Testing sscanf() : error conditions ***

-- Testing sscanf() function with more than expected no. of arguments --
ValueError: Variable is not assigned by any conversion specifiers
