--TEST--
Bug #37017 (strtotime fails before 13:00:00 with some time zones identifiers).
--INI--
date.timezone=GMT
--FILE--
<?php
echo strtotime("2006-05-12 13:00:01 America/New_York"), "\n";
echo strtotime("2006-05-12 13:00:00 America/New_York"), "\n";
echo strtotime("2006-05-12 12:59:59 America/New_York"), "\n";
echo strtotime("2006-05-12 12:59:59 GMT"), "\n";
?>
--EXPECT--
1147453201
1147453200
1147453199
1147438799
