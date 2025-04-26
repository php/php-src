--TEST--
GH-18439 (Reference handling in cancel callback)
--EXTENSIONS--
zip
--FILE--
<?php

function &cb() {
    $test = false;
    return $test;
}

$file = __DIR__ . '/gh18439.zip';
$zip = new ZipArchive;
$zip->open($file, ZIPARCHIVE::CREATE);
$zip->registerCancelCallback(cb(...));
$zip->addFromString('test', 'test');
echo "Done\n";

?>
--CLEAN--
<?php
$file = __DIR__ . '/gh18439.zip';
@unlink($file);
?>
--EXPECT--
Done
