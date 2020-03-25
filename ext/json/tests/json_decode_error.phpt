--TEST--
Test json_decode() function : error conditions
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
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
Depth must be greater than zero
