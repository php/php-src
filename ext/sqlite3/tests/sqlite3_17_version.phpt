--TEST--
SQLite3::version()
--SKIPIF--
<?php require 'skipif.inc'; ?>
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
