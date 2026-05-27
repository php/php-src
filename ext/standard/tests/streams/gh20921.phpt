--TEST--
GH-20921: SplTempFileObject::fseek with PHP_INT_MIN must not trigger UB
--FILE--
<?php
$f = new SplTempFileObject();
$f->fwrite("abcdef");

var_dump($f->fseek(PHP_INT_MIN, SEEK_END));

?>
--EXPECT--
int(-1)
