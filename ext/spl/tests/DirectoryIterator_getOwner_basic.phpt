--TEST--
SPL: DirectoryIterator test getOwner
--SKIPIF--
<?php
if (posix_geteuid() == 0) die('SKIP Cannot run test as root.');
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
$dirname = 'DirectoryIterator_getOwner_basic';
mkdir($dirname);
$dir = new DirectoryIterator($dirname);
$expected = fileowner($dirname);
$actual = $dir->getOwner();
var_dump($expected == $actual);
?>
--CLEAN--
<?php
$dirname = 'DirectoryIterator_getOwner_basic';
rmdir($dirname);
?>
--EXPECT--
bool(true)
