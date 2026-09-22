--TEST--
ZipArchive::closeString() false return
--EXTENSIONS--
zip
--FILE--
<?php
$zip = new ZipArchive();
// The "compressed size" fields are wrong, causing an error when reading the contents.
// The error is reported on close when we rewrite the member with setCompressionIndex().
// The error code is ER_DATA_LENGTH in libzip 1.10.0+ or ER_INCONS otherwise.
$input = file_get_contents(__DIR__ . '/wrong-file-size.zip');
var_dump($zip->openString($input));
$zip->setCompressionIndex(0, ZipArchive::CM_DEFLATE);
var_dump($zip->closeString());
echo $zip->getStatusString() . "\n";
?>
--EXPECTREGEX--
bool\(true\)

Warning: ZipArchive::closeString\(\): (Zip archive inconsistent|Unexpected length of data).*
bool\(false\)
(Zip archive inconsistent|Unexpected length of data).*
