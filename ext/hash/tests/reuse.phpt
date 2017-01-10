--TEST--
Attempt to reuse a closed hash context
--SKIPIF--
<?php if (!extension_loaded('hash')) echo 'skip';
--FILE--
<?php

$h = hash_init('md5');
echo hash_final($h), "\n";
hash_update($h, 'foo');
echo hash_final($h), "\n";
--EXPECTF--
d41d8cd98f00b204e9800998ecf8427e
acbd18db4cc2f85cedef654fccc4a4d8