--TEST--
finfo_file(): Files and directories inside an stream
--SKIPIF--
<?php require_once(__DIR__ . '/skipif.inc'); ?>
<?php if (!extension_loaded('zip')) { die("skip zip extension not loaded"); } ?>
--FILE--
<?php

$fp = finfo_open(FILEINFO_MIME_TYPE);
$results = array();

$zip = __DIR__ . "/resources/dir.zip";
$stream = "zip://" . __DIR__ . "/resources/dir.zip";
$dir = $stream . "#dir/";
$png = $stream . "#dir/test.png";

var_dump(
         finfo_file($fp, $zip),
         finfo_file($fp, $dir),
         finfo_file($fp, $png)
);
?>
--EXPECT--
string(15) "application/zip"
string(9) "directory"
string(9) "image/png"
