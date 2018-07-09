--TEST--
Bug #71504: Parsing of tar file with duplicate filenames causes memory leak
--SKIPIF--
<?php if (!extension_loaded('phar')) die('skip'); ?>
<?php if (!extension_loaded("zlib")) die("skip zlib not available"); ?>
--FILE--
<?php
$fname = str_replace('\\', '/', dirname(__FILE__) . '/files/HTML_CSS-1.5.4.tgz');
try {
	$tar = new PharData($fname);
} catch(Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--EXPECT--
===DONE===
