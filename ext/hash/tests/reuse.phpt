--TEST--
Attempt to reuse a closed hash context
--SKIPIF--
<?php if (!extension_loaded('hash')) echo 'skip';
--FILE--
<?php

$h = hash_init('md5');
hash_final($h);
hash_update($h, 'foo');
--EXPECTF--
Warning: hash_update(): supplied resource is not a valid Hash Context resource in %s%eext%ehash%etests%ereuse.php on line %d
