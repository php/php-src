--TEST--
Phar: test a zip archive created by openoffice
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
<?php if (!extension_loaded("spl")) die("skip SPL not available"); ?>
<?php if (!extension_loaded("zlib")) die("skip zlib not available"); ?>
--FILE--
<?php
$a = new PharData(dirname(__FILE__) . '/files/odt.odt');
foreach (new RecursiveIteratorIterator($a, RecursiveIteratorIterator::LEAVES_ONLY) as $b) {
	if ($b->isDir()) {
		echo "dir " . $b->getPathName() . "\n";
	} else {
		echo $b->getPathName() . "\n";
	}
}
// this next line is for increased code coverage
try {
	$b = new Phar(dirname(__FILE__) . '/files/odt.odt');
} catch (Exception $e) {
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--EXPECTF--
phar://%sodt.odt/Configurations2/accelerator%ccurrent.xml
phar://%sodt.odt/META-INF%cmanifest.xml
phar://%sodt.odt/Thumbnails%cthumbnail.png
phar://%sodt.odt%ccontent.xml
phar://%sodt.odt%cmeta.xml
phar://%sodt.odt%cmimetype
phar://%sodt.odt%csettings.xml
phar://%sodt.odt%cstyles.xml
Cannot create phar '%sodt.odt', file extension (or combination) not recognised
===DONE===
