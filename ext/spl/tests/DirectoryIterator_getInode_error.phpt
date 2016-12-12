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
$fileInfo = new SplFileInfo('not_existing');
var_dump($fileInfo->getInode());
?>

--EXPECTF--
Fatal error: Uncaught RuntimeException: SplFileInfo::getInode(): stat failed for %s in %s
Stack trace:
#0 %s: SplFileInfo->getInode()
#1 {main}
  thrown in %s
