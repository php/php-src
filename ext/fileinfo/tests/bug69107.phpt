--TEST--
Bug #69107 (finfo no longer detects PHP files)
--EXTENSIONS--
fileinfo
--FILE--
<?php
$finfo = new finfo(FILEINFO_MIME_TYPE);

var_dump($finfo->buffer("<?php\nclass A{}"));
var_dump($finfo->buffer("<?php class A{}"));
var_dump($finfo->buffer("<?php\tclass A{}"));
var_dump($finfo->buffer("<?php\n\rclass A{}"));
?>
--EXPECT--
string(10) "text/x-php"
string(10) "text/x-php"
string(10) "text/x-php"
string(10) "text/x-php"
