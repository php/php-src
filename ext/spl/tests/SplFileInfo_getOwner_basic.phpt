--TEST--
SPL: SplFileInfo test getOwner
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
$filename = __DIR__ . "/SplFileInfo_getOwner_basic";
touch($filename);
$fileInfo = new SplFileInfo($filename);
$expected = fileowner($filename);
$actual = $fileInfo->getOwner();
var_dump($expected == $actual);
?>
--CLEAN--
<?php
$filename = __DIR__ . "/SplFileInfo_getOwner_basic";
unlink($filename);
?>
--EXPECT--
bool(true)
