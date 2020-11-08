--TEST--
Bug #78976 (SplFileObject::fputcsv returns -1 on failure)
--FILE--
<?php
$file = new SplFileObject('php://memory', 'r');
var_dump($file->fputcsv(['foo', 'bar']));
?>
--EXPECT--
bool(false)
