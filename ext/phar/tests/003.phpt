--TEST--
Phar::mapPhar zlib is loaded
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip zlib is present"; ?>
--FILE--
<?php
/* check this works and actually returns a boolean value */
var_dump(Phar::canCompress());
?>
--EXPECTF--
bool(%s)
