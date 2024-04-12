--TEST--
Test gzdeflate() function : variation
--EXTENSIONS--
zlib
--FILE--
<?php
include(__DIR__ . '/data.inc');

echo "*** Testing gzdeflate() : variation ***\n";



echo "\n-- Testing multiple compression --\n";
$output = gzdeflate($data);
var_dump(strlen($output));
var_dump(strlen(gzdeflate($output)));

?>
--EXPECTF--
*** Testing gzdeflate() : variation ***

-- Testing multiple compression --
int(17%d)
int(17%d)
