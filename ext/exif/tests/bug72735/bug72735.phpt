--TEST--
Bug #72735 (Samsung picture thumb not read (zero size))
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--FILE--
<?php
foreach (['nokia.jpg', 'samsung.jpg'] as $picture) {
	$len = strlen(exif_thumbnail(__DIR__ . '/' . $picture));

	if ($len === 0) {
		echo $picture . ': error, no thumbnail length', PHP_EOL;

		continue;
	}

	echo $picture . ': int(' . $len . ')', PHP_EOL;
}
?>
--EXPECTF--
nokia.jpg: int(%d)
samsung.jpg: int(%d)
