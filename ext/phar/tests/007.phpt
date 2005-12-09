--TEST--
PHP_Archive::mapPhar manifest too big
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip";?>
--FILE--
<?php
PHP_Archive::mapPhar(5, 'hio', false);
__HALT_COMPILER(); ?>~~~~
--EXPECTF--
Fatal error: PHP_Archive::mapPhar(): manifest cannot be larger than 1 MB in phar "%s" in %s on line %d