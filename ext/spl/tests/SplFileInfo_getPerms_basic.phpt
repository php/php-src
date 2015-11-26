--TEST--
SPL: Spl File Info test getPerms
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
touch ('SplFileInfo_getPerms_basic.txt');
chmod('SplFileInfo_getPerms_basic.txt', 0557);
$fileInfo = new SplFileInfo('SplFileInfo_getPerms_basic.txt');
var_dump($fileInfo->getPerms() == 0100557);

?>
--CLEAN--
<?php
unlink('SplFileInfo_getPerms_basic.txt');
?>
--EXPECTF--
bool(true)
