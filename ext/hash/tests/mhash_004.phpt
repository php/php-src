--TEST--
MHash: mhash() modifying algorithm parameter
--SKIPIF--
<?php if(!function_exists('mhash')) { die('skip mhash compatibility layer not available'); } ?>
--FILE--
<?php

$algo = MHASH_MD5;
var_dump($algo);
var_dump(bin2hex(mhash($algo, "test")));
var_dump($algo);

?>
--EXPECTF--
int(1)

Deprecated: Function mhash() is deprecated in %s on line %d
string(32) "098f6bcd4621d373cade4e832627b4f6"
int(1)
