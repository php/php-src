--TEST--
Bug #75102 (`PharData` says invalid checksum for valid tar)
--EXTENSIONS--
phar
--FILE--
<?php
$phar = new PharData(__DIR__ . '/bug75102.tar');
var_dump(file_get_contents($phar['test.txt']->getPathName()));
?>
--EXPECT--
string(9) "yada yada"
