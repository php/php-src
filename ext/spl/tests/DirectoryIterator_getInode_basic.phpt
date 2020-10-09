--TEST--
SPL: Spl Directory Iterator test getInode
--CREDITS--
Cesare D'Amico <cesare.damico@gruppovolta.it>
Andrea Giorgini <agiorg@gmail.com>
Filippo De Santis <fd@ideato.it>
Daniel Londero <daniel.londero@gmail.com>
Francesco Trucchia <ft@ideato.it>
Jacopo Romei <jacopo@sviluppoagile.it>
#Test Fest Cesena (Italy) on 2009-06-20
--SKIPIF--
<?php
if (PHP_OS_FAMILY === 'Windows') { die('SKIP Testing file inodes, not available for Windows'); }
?>
--FILE--
<?php

//file
mkdir('test_dir_ptfi');
$dirIterator = new DirectoryIterator('test_dir_ptfi');
var_dump(decoct($dirIterator->getInode()));

?>
--CLEAN--
<?php
rmdir('test_dir_ptfi');
?>
--EXPECTF--
string(%d) "%d"
