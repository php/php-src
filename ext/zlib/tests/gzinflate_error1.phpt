--TEST--
Test gzinflate() function : error conditions
--EXTENSIONS--
zlib
--FILE--
<?php
include(__DIR__ . '/data.inc');

echo "*** Testing gzinflate() : error conditions ***\n";

echo "\n-- Testing with a buffer that is too small --\n";
$data = 'string_val';
$short_len = strlen($data) - 1;
$compressed = gzcompress($data);

try {
    var_dump(gzinflate($compressed, $short_len));
} catch (\Throwable $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECTF--
*** Testing gzinflate() : error conditions ***

-- Testing with a buffer that is too small --
data error
