--TEST--
memory stream seek edge case: PHP_INT_MIN with SEEK_END
--FILE--
<?php
$cls = new SplTempFileObject();
$fusion = $cls;

var_dump($fusion->fseek(PHP_INT_MIN, SEEK_END));
--EXPECT--
int(-1)
