--TEST--
Leaking ZipArchive destructor
--EXTENSIONS--
zip
--FILE--
<?php

class MyZipArchive extends ZipArchive {
    public function __destruct() {
        global $leak;
        $leak = $this;
    }
}

new MyZipArchive;
$file = __DIR__ . '/ZipArchive_destruct.zip';
$leak->open($file, ZIPARCHIVE::CREATE);
$leak->addFromString('test', 'test');

?>
===DONE===
--CLEAN--
<?php
$file = __DIR__ . '/ZipArchive_destruct.zip';
@unlink($file);
?>
--EXPECT--
===DONE===
