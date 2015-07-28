--TEST--
Recognition of compression methods
--DESCRIPTION--
This test is supposed to cover all compression methods that are recognized by
libzip, but for now only 6 methods are available in compression_methods.zip.
The zip and the test should be extended, if possible.
--SKIPIF--
<?php
if (!extension_loaded('zip')) die('skip zip extension not available');
?>
--FILE--
<?php
$methods = array(
    'store'     => ZipArchive::CM_STORE,
    'deflate'   => ZipArchive::CM_DEFLATE,
    'deflate64' => ZipArchive::CM_DEFLATE64,
    'bzip2'     => ZipArchive::CM_BZIP2,
    'lzma'      => ZipArchive::CM_LZMA,
    'ppmd'      => ZipArchive::CM_PPMD
);
$zip = new ZipArchive();
$zip->open(__DIR__ . '/compression_methods.zip');
foreach ($methods as $filename => $method) {
    echo "$filename: ";
    var_dump($zip->statName($filename)['comp_method'] === $method);
}
$zip->close();
?>
--EXPECT--
store: bool(true)
deflate: bool(true)
deflate64: bool(true)
bzip2: bool(true)
lzma: bool(true)
ppmd: bool(true)
