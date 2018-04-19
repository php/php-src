--TEST--
Phar: tar archive, require_hash=1, OpenSSL hash
--SKIPIF--
<?php if (!extension_loaded('phar')) die('skip'); ?>
<?php if (!extension_loaded("zlib")) die("skip zlib not available"); ?>
<?php if (!extension_loaded("openssl")) die("skip openssl not available"); ?>
--INI--
phar.readonly=1
phar.require_hash=1
--FILE--
<?php
try {
	$phar = new PharData(dirname(__FILE__) . '/files/P1-1.0.0.tgz');
} catch (Exception $e) {
	echo $e->getMessage()."\n";
}

?>
===DONE===
--EXPECT--
===DONE===
