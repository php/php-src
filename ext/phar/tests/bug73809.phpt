--TEST--
Bug #73809 (Phar Zip parse crash - mmap fail)
--EXTENSIONS--
phar
zip
--FILE--
<?php
// create the ZIP to be tested
$zip = new ZipArchive;
$zip->open(__DIR__ . '/73809.zip', ZipArchive::CREATE);
$zip->addFromString('73809.txt', 'yada yada');
$zip->addFromString('.phar/signature.bin', str_repeat('*', 64 * 1024 + 1));
$zip->setCompressionName('.phar/signature.bin', ZipArchive::CM_STORE);
var_dump($zip->close());

try {
    $phar = new PharData(__DIR__ . '/73809.zip');
} catch (Exception $ex) {
    echo $ex->getMessage(), PHP_EOL;
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/73809.zip');
?>
--EXPECTF--
bool(true)
phar error: signatures larger than 64 KiB are not supported in zip-based phar "%s"
