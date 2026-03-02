--TEST--
Phar: bug #46032: PharData::__construct wrong memory read
--EXTENSIONS--
phar
--SKIPIF--
<?php if (getenv('SKIP_SLOW_TESTS')) die('skip'); ?>
--FILE--
<?php

$a = __DIR__ .'/mytest';

try {
    new phar($a);
} catch (exception $e) { }

var_dump($a);

try {
    new phar($a);
} catch (exception $e) { }

var_dump($a);

new phardata('0000000000000000000');
?>
===DONE===
--EXPECTF--
string(%d) "%smytest"
string(%d) "%smytest"

Fatal error: Uncaught UnexpectedValueException: Cannot create phar '0000000000000000000', file extension (or combination) not recognised or the directory does not exist in %sbug46032.php:%d
Stack trace:
#0 %sbug46032.php(%d): PharData->__construct('000000000000000...')
#1 {main}
  thrown in %sbug46032.php on line %d
