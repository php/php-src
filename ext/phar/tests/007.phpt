--TEST--
Phar::mapPhar manifest too big
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip";?>
--FILE--
<?php
Phar::mapPhar('hio');
__HALT_COMPILER(); ?>~~~~
--EXPECTF--
%satal error: Phar::mapPhar(): manifest cannot be larger than 1 MB in phar "%s" in %s on line %d
