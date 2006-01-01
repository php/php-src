--TEST--
PHP_Archive::mapPhar zlib not loaded
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip zlib is present";
if (Phar::canCompress()) print "skip";?>
--FILE--
<?php
Phar::mapPhar(5, 'hio', true);
?>
--EXPECTF--
Fatal error: Phar::mapPhar(): zlib extension is required for compressed .phar files