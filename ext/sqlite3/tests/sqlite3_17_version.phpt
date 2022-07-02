--TEST--
SQLite3::version()
--EXTENSIONS--
sqlite3
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
