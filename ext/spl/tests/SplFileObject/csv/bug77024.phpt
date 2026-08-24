--TEST--
Bug #77024 SplFileObject::__toString() may return array
--FILE--
<?php

$file = new SplTempFileObject;
$file->setCsvControl(escape: "");
$file->fputcsv(['foo', 'bar', 'baz']);
$file->rewind();
$file->setFlags(SplFileObject::READ_CSV);
echo $file . "\n";

$tmp = tempnam(sys_get_temp_dir(), "php-tests-");
file_put_contents($tmp, "line1\nline2\nline3\n");
$file = new SplFileObject($tmp);
$file->rewind();
echo $file . "\n";
unset($file);
unlink($tmp);

?>
--EXPECTF--
Deprecated: Method SplFileObject::setCsvControl() is deprecated since 8.6 in %s on line %d

Deprecated: Method SplFileObject::fputcsv() is deprecated since 8.6 in %s on line %d
foo,bar,baz

line1
