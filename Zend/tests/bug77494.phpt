--TEST--
Bug #77494 (Disabling class causes segfault on member access)
--EXTENSIONS--
curl
--INI--
disable_classes=CURLFile,ErrorException
--FILE--
<?php
$a = new CURLFile();
var_dump($a->name);
$b = new ErrorException();
var_dump($b->message);
?>
--EXPECTF--
Warning: CURLFile() has been disabled for security reasons in %sbug77494.php on line 2

Warning: Undefined property: CURLFile::$name in %s on line %d
NULL

Warning: ErrorException() has been disabled for security reasons in %s on line %d

Warning: Undefined property: ErrorException::$message in %s on line %d
NULL
