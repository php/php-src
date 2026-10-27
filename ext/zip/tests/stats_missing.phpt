--TEST--
Regression tests for behavior when the requested zip does not exist
--EXTENSIONS--
zip
--FILE--
<?php
$target = __DIR__ . '/target.zip';
assert(!file_exists($target), 'target.zip should not exist at the start');

echo "Stat:\n";
var_dump(stat("zip://$target#entry.txt"));
assert(!file_exists($target), 'target.zip should not exist after stat()');

echo "\n\nFile contents:\n";
var_dump(file_get_contents("zip://$target#entry.txt"));
assert(!file_exists($target), 'target.zip should not exist after file_get_contents()');

echo "\n\nFopen:\n";
var_dump(fopen("zip://$target#entry.txt", 'r'));
assert(!file_exists($target), 'target.zip should not exist after fopen() for reading');

echo "\n\nOn a missing directory:\n";
var_dump(stat('zip:///nonexistent_dir_xyz/target.zip#entry.txt'));
?>
--EXPECTF--
Stat:

Warning: stat(): stat failed for zip://%starget.zip#entry.txt in %s on line %d
bool(false)


File contents:

Warning: file_get_contents(): Failed to open stream: operation failed in %s on line %d
bool(false)


Fopen:

Warning: fopen(): Failed to open stream: operation failed in %s on line %d
bool(false)


On a missing directory:

Warning: stat(): stat failed for zip:///nonexistent_dir_xyz/target.zip#entry.txt in %s on line %d
bool(false)
