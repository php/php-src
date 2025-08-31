--TEST--
Bug #69320 libmagic crash when running laravel tests
--EXTENSIONS--
fileinfo
--FILE--
<?php

$fname = __DIR__ . DIRECTORY_SEPARATOR . "bug69320.txt";
file_put_contents($fname, "foo");
var_dump(finfo_file(finfo_open(FILEINFO_MIME_TYPE), $fname));

?>
--CLEAN--
<?php
    $fname = __DIR__ . DIRECTORY_SEPARATOR . "bug69320.txt";
    unlink($fname);
?>
--EXPECT--
string(10) "text/plain"
