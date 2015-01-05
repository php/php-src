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
		echo $b->getPathName(), "\n";
		echo file_get_contents($b->getPathName()), "\n";
	}
}
if (isset($a['notempty/hi.txt'])) {
	echo $a['notempty/hi.txt']->getPathName() . "\n";
}
?>
===DONE===
--EXPECTF--
dir phar://%s/zip.zip%cempty
phar://%s/zip.zip%chi.txt
hi there

dir phar://%s/zip.zip%cnotempty
phar://%s/zip.zip/notempty%chi.txt
===DONE===
