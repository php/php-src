--TEST--
SPL: Spl File Info test getInode
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

//file
touch ('test_file_ptfi');
$fileInfo = new SplFileInfo('test_file_ptfi');
$result = shell_exec('ls -i test_file_ptfi');
var_dump($fileInfo->getInode() == $result);

?>
--CLEAN--
<?php
unlink('test_file_ptfi');
?>
--EXPECTF--
bool(true)
