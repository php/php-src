--TEST--
Regression tests for behavior when the requested zip is not a zip file
--EXTENSIONS--
zip
--FILE--
<?php
$target = __FILE__;

echo "Stat:\n";
var_dump(stat("zip://$target#entry1.txt"));

echo "\n\nFile contents:\n";
var_dump(file_get_contents("zip://$target#entry1.txt"));
?>
--EXPECTF--
Stat:

Warning: stat(): stat failed for zip://%sstats_nonzip.php#entry1.txt in %s on line %d
bool(false)


File contents:

Warning: file_get_contents(): Failed to open stream: operation failed in %s on line %d
bool(false)
