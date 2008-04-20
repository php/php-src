--TEST--
Phar: test a zip archive created by unix "zip" command
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
--FILE--
<?php
$a = new PharData(dirname(__FILE__) . '/files/zip.zip');
foreach ($a as $b) {
	if ($b->isDir()) {
		echo "dir " . $b->getPathName() . "\n";
	} else {
		echo $b->getPathName() . "\n";
	}
}
if (isset($a['notempty/hi.txt'])) {
	echo $a['notempty/hi.txt']->getPathName() . "\n";
}
?>
===DONE===
--EXPECTF--
dir phar://%szip.zip%cempty
phar://%szip.zip%chi.txt
dir phar://%szip.zip%cnotempty
phar://%szip.zip/notempty/hi.txt
===DONE===
