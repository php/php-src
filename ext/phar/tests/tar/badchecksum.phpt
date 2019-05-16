--TEST--
Phar: tar with bad checksum
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
$fname = __DIR__ . '/' . basename(__FILE__, '.php') . '.tar';
$pname = 'phar://' . $fname;

include __DIR__ . '/files/corrupt_tarmaker.php.inc';
$a = new corrupt_tarmaker($fname, 'none');
$a->init();
$a->addFile('hithere', 'contents', null, 'checksum');
$a->close();

try {
	$p = new PharData($fname);
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}

?>
===DONE===
--CLEAN--
<?php
unlink(__DIR__ . '/' . basename(__FILE__, '.clean.php') . '.tar');
?>
--EXPECTF--
phar error: "%sbadchecksum.tar" is a corrupted tar file (checksum mismatch of file "hithere")
===DONE===
