--TEST--
SPL: DirectoryIterator test getGroup
--CREDITS--
Cesare D'Amico <cesare.damico@gruppovolta.it>
Andrea Giorgini <agiorg@gmail.com>
Filippo De Santis <fd@ideato.it>
Daniel Londero <daniel.londero@gmail.com>
Francesco Trucchia <ft@ideato.it>
Jacopo Romei <jacopo@sviluppoagile.it>
#Test Fest Cesena (Italy) on 2009-06-20
--FILE--
<?php
$dirname = basename(__FILE__, '.phpt');
mkdir($dirname);
$dir = new DirectoryIterator($dirname);
$expected = filegroup($dirname);
$actual = $dir->getGroup();
var_dump($expected == $actual);
?>
--CLEAN--
<?php
$dirname = basename(__FILE__, '.phpt');
rmdir($dirname);
?>
--EXPECTF--
bool(true)
