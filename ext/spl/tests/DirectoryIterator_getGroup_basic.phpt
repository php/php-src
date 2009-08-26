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
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') die("skip this test not for Windows platforms");
?>
--FILE--
<?php

shell_exec('mkdir test_dir_ptfi');
$dir = new DirectoryIterator('test_dir_ptfi');
$result = shell_exec('ls -lnd test_dir_ptfi | cut -d" " -f 4');

var_dump($dir->getGroup() == $result);

?>
--CLEAN--
<?php
rmdir('test_dir_ptfi');
?>
--EXPECTF--
bool(true)
