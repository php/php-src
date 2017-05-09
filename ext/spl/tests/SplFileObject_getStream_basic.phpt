--TEST--
SplFileObject::getStream function - basic test
--FILE--
<?php
/*
 * test a successful read from an SplFileObject's stream
*/
$obj = new SplTempFileObject;
$obj->fwrite("test content");

$fh = $obj->getStream();
fseek($fh, 0);
var_dump(fread($fh, 20));

?>
--EXPECTF--
string(12) "test content"
