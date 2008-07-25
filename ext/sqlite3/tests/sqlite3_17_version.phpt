--TEST--
SQLite3::version()
--SKIPIF--
<?php require_once(dirname(__FILE__) . '/skipif.inc'); ?>
--FILE--
<?php
print_r(SQLite3::version());
echo "Done\n";
?>
--EXPECTF--
Array
(
    [versionString] => %d.%d.%d
    [versionNumber] => %d
)
Done
