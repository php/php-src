--TEST--
Memory stream seek overflow detection and position preservation on failure
--FILE--
<?php
$f = fopen('php://memory', 'r+');
fwrite($f, "hello world");
var_dump(ftell($f));

var_dump(fseek($f, 5, SEEK_SET));
var_dump(ftell($f));

// Seeking past ZEND_LONG_MAX with SEEK_END fails and preserves position
var_dump(fseek($f, PHP_INT_MAX, SEEK_END));
var_dump(ftell($f));

// Seeking with positive offset from SEEK_END succeeds
var_dump(fseek($f, 10, SEEK_END));
var_dump(ftell($f));

fclose($f);
?>
--EXPECT--
int(11)
int(0)
int(5)
int(-1)
int(5)
int(0)
int(21)
