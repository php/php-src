--TEST--
SQLite3::version()
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
--FILE--
<?php
print_r(SQLite3::version());
echo "Done\n";
?>
--EXPECTF--
Array
(
    [versionString] => %s
    [versionNumber] => %d
)
Done
