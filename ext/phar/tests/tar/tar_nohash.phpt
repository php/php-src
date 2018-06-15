--TEST--
Phar: tar archive, require_hash=1, should not error out
--SKIPIF--
<?php if (!extension_loaded('phar')) die('skip'); ?>
<?php if (!extension_loaded("zlib")) die("skip zlib not available"); ?>
--INI--
phar.readonly=1
phar.require_hash=1
--FILE--
<?php
try {
	$phar = new PharData(dirname(__FILE__) . '/files/Net_URL-1.0.15.tgz');
	var_dump($phar->getStub());
} catch (Exception $e) {
	echo $e->getMessage()."\n";
}

?>
===DONE===
--EXPECT--
string(0) ""
===DONE===
