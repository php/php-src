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
--FILE--
<?php

//file
$fileInfo = new SplFileInfo('not_existing');
var_dump($fileInfo->getInode());
?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: SplFileInfo::getInode(): stat failed for not_existing in %s
Stack trace:
#0 %s: SplFileInfo->getInode()
#1 {main}
  thrown in %s on line %d
