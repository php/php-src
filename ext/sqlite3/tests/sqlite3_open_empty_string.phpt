--TEST--
SQLite3::open test with empty string argument via the constructor
--CREDITS--
Thijs Feryn <thijs@feryn.eu>
#TestFest PHPBelgium 2009
--EXTENSIONS--
sqlite3
--FILE--
<?php
$db = new SQLite3('');
echo "Done\n";
?>
--EXPECT--
Done
