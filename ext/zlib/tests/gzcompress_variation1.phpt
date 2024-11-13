--TEST--
Test gzcompress() function : variation
--EXTENSIONS--
zlib
--FILE--
<?php
include(__DIR__ . '/data.inc');

echo "*** Testing gzcompress() : variation ***\n";

echo "\n-- Testing multiple compression --\n";
$output = gzcompress($data);
var_dump(strlen($output));
var_dump(strlen(gzcompress($output)));

?>
--EXPECTF--
*** Testing gzcompress() : variation ***

-- Testing multiple compression --
int(1%d)
int(1%d)
