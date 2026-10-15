--TEST--
Regression tests for behavior when the requested zip does exist
--EXTENSIONS--
zip
--FILE--
<?php
$target = __DIR__ . '/test.zip';

echo "Stat:\n";
var_dump(stat("zip://$target#entry1.txt"));

echo "\n\nFile contents:\n";
var_dump(file_get_contents("zip://$target#entry1.txt"));
?>
--EXPECTF--
Stat:

Warning: stat(): stat failed for zip://%stest.zip#entry1.txt in %s on line %d
bool(false)


File contents:
string(8) "entry #1"
