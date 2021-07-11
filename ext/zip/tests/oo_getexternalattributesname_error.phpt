--TEST--
ZipArchive::getExternalAttributesName() empty name error
--DESCRIPTION--
This test verifies that ZipArchive::getExternalAttributesName() will throw Fatal error: Uncaught ValueError if the first parameter is empty
--CREDITS--
Meletios Flevarakis php@flevarakis.xyz
--EXTENSIONS--
zip
--FILE--
<?php

$zip = new ZipArchive();
$dirname = __DIR__ . '/';

$zip->open($dirname . "test.zip", ZIPARCHIVE::CREATE);

$a = ZipArchive::OPSYS_DEFAULT;
$b = ZipArchive::OPSYS_DEFAULT;

$out = $zip->getExternalAttributesName("", $a, $b);

?>
--EXPECTF--
Fatal error: Uncaught ValueError: ZipArchive::getExternalAttributesName(): Argument #1 ($name) cannot be empty in %s
Stack trace:
#0 %s(%d): ZipArchive->getExternalAttributesName('', 3, 3)
#1 {main}
  thrown in %s on line %d
