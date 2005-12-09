--TEST--
PHP_Archive::mapPhar zlib not loaded
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip";
if (PHP_Archive::canCompress()) print "skip";?>
--FILE--
<?php
PHP_Archive::mapPhar(5, 'hio', true);
?>
--EXPECTF--
Fatal error: PHP_Archive::mapPhar(): zlib extension is required for compressed .phar files