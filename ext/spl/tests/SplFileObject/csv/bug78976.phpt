--TEST--
Bug #78976 (SplFileObject::fputcsv returns -1 on failure)
--FILE--
<?php
$file = new SplFileObject('php://memory', 'r');
$file->setCsvControl(escape: "");
var_dump($file->fputcsv(['foo', 'bar']));
?>
--EXPECTF--
Deprecated: Method SplFileObject::setCsvControl() is deprecated since 8.6 in %s on line %d

Deprecated: Method SplFileObject::fputcsv() is deprecated since 8.6 in %s on line %d
bool(false)
