--TEST--
Hash: Attempt to reuse a closed hash context
--FILE--
<?php

$h = hash_init('md5');
hash_final($h);
hash_update($h, 'foo');
--EXPECTF--
Warning: hash_update(): supplied resource is not a valid Hash Context resource in %s%eext%ehash%etests%ereuse.php on line %d
