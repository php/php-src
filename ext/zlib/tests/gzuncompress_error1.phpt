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

var_dump(gzuncompress($compressed, $short_len));

echo "\n-- Testing with incorrect arguments --\n";
var_dump(gzuncompress(123));

?>
--EXPECTF--
*** Testing gzuncompress() : error conditions ***

-- Testing with a buffer that is too small --

Warning: gzuncompress(): insufficient memory in %s on line %d
bool(false)

-- Testing with incorrect arguments --

Warning: gzuncompress(): data error in %s on line %d
bool(false)
