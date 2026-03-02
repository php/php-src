--TEST--
GH-19688 (Remove pattern overflow in zip addGlob())
--EXTENSIONS--
zip
--FILE--
<?php
$dir = __DIR__ . '/';
$testfile = $dir . '001.phpt';
$zip = new ZipArchive();
$filename = $dir . '/gh19688.zip';
$zip->open($filename, ZipArchive::CREATE | ZipArchive::OVERWRITE);
$options = array('remove_path' => $dir . 'a very long string here that will overrun');
$zip->addGlob($testfile, 0, $options);
var_dump($zip->getNameIndex(0));
?>
--CLEAN--
<?php
@unlink(__DIR__  . '/gh19688.zip');
?>
--EXPECTF--
string(%d) "%s001.phpt"
