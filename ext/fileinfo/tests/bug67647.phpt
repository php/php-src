--TEST--
Bug #67647: Bundled libmagic 5.17 does not detect quicktime files correctly
--EXTENSIONS--
fileinfo
--FILE--
<?php

$f = __DIR__ . DIRECTORY_SEPARATOR . "67647.mov";

$fi = new finfo(FILEINFO_MIME_TYPE);
var_dump($fi->file($f));
?>
+++DONE+++
--EXPECT--
string(15) "video/quicktime"
+++DONE+++
