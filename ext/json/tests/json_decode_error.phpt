--TEST--
Test json_decode() function : error conditions
--FILE--
<?php
echo "*** Testing json_decode() : error conditions ***\n";

echo "\n-- Testing json_decode() function with depth below 0 --\n";

try {
    var_dump(json_decode('"abc"', true, -1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
*** Testing json_decode() : error conditions ***

-- Testing json_decode() function with depth below 0 --
json_decode(): Argument #3 ($depth) must be greater than 0
