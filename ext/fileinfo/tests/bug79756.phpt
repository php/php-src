--TEST--
Bug #79756 (finfo_file crash (FILEINFO_MIME))
--SKIPIF--
<?php
if (!extension_loaded('fileinfo')) die('skip fileinfo extension not available');
?>
--FILE--
<?php
$filename = __DIR__ . '/bug79756.xls';
$finfo = finfo_open(FILEINFO_MIME);
$mime = finfo_file($finfo, $filename);
finfo_close($finfo);
echo $mime;
?>
--EXPECT--
application/vnd.ms-excel; charset=binary
