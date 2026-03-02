--TEST--
Bug #79756 (finfo_file crash (FILEINFO_MIME))
--EXTENSIONS--
fileinfo
--FILE--
<?php
$filename = __DIR__ . '/bug79756.xls';
$finfo = finfo_open(FILEINFO_MIME);
$mime = finfo_file($finfo, $filename);
echo $mime;
?>
--EXPECT--
application/vnd.ms-excel; charset=binary
