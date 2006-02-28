--TEST--
Phar::mapPhar truncated manifest (none)
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip";?>
--FILE--
<?php
Phar::mapPhar('hio');
__HALT_COMPILER(); ?>()
--EXPECTF--
%satal error: Phar::mapPhar(): internal corruption of phar "%s" (truncated manifest) in %s on line %d
