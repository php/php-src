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
--EXPECTF--
Warning: Undefined variable $leak in %s on line 12

Fatal error: Uncaught Error: Call to a member function open() on null in %s:12
Stack trace:
#0 {main}
  thrown in %s on line 12
