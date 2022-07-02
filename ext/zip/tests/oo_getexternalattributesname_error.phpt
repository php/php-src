--TEST--
ZipArchive::getExternalAttributesName() throws a ValueError when the $name param is empty
--EXTENSIONS--
zip
--FILE--
<?php

$zip = new ZipArchive();
$dirname = __DIR__ . '/';

$zip->open($dirname . "test.zip", ZIPARCHIVE::CREATE);

$a = ZipArchive::OPSYS_DEFAULT;
$b = ZipArchive::OPSYS_DEFAULT;

try {
    $zip->getExternalAttributesName("", $a, $b);
} catch(ValueError $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
ZipArchive::getExternalAttributesName(): Argument #1 ($name) cannot be empty
