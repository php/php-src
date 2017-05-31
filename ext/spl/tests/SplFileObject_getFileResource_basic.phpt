--TEST--
SPL: SplFileObject test getFileResource

--FILE--
<?php

// Test via file
$filename = __DIR__ . "/SplFileObject_getFileResource_basic";
$testString = "This is a test.";
file_put_contents($filename, $testString);
$fileInfo = new SplFileObject($filename);
$fileResource = $fileInfo->getFileResource();
$readString = fread($fileResource, strlen($testString));
var_dump($testString == $readString);

// Test via stdout
$testString = "This is a test.";
$fileInfo = new SplFileObject("php://output");
$fileResource = $fileInfo->getFileResource();
fwrite($fileResource, "This is a test.");

?>
--CLEAN--
<?php
$filename = __DIR__ . "/SplFileObject_getFileResource_basic";
unlink($filename);
?>
--EXPECTF--
bool(true)
This is a test.