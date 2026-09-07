--TEST--
Phar::mapPhar manifest too big
--EXTENSIONS--
phar
--FILE--
<?php
try {
Phar::mapPhar('hio');
} catch (Exception $e) {
echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
__HALT_COMPILER(); ?>~~~~
--EXPECTF--
PharException: manifest cannot be larger than 100 MB in phar "%s"
