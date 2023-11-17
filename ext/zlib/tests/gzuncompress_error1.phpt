--TEST--
Test gzuncompress() function : error conditions
--EXTENSIONS--
zlib
--FILE--
<?php
echo "*** Testing gzuncompress() : error conditions ***\n";

echo "\n-- Testing with a buffer that is too small --\n";
$data = 'string_val';
$short_len = strlen($data) - 1;
$compressed = gzcompress($data);

try {
    var_dump(gzuncompress($compressed, $short_len));
} catch (\Throwable $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "\n-- Testing with incorrect arguments --\n";
try {
    var_dump(gzuncompress(123));
} catch (\Throwable $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECTF--
*** Testing gzuncompress() : error conditions ***

-- Testing with a buffer that is too small --
insufficient memory

-- Testing with incorrect arguments --
data error
