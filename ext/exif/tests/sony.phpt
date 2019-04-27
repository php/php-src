--TEST--
Sony test
--SKIPIF--
<?php if (!extension_loaded('exif')) print 'skip exif extension not available';?>
--FILE--
<?php
$data = exif_read_data(__DIR__ . DIRECTORY_SEPARATOR . 'sony.jpg');

if (!$data) {
	exit('Error: Unable to parse EXIF data');
}

// Perhaps we should just test for SonyModelID since it seems to be
// the most specific tag name that should be found in any Sony generated
// picture
foreach (['SonyModelID', 'Panorama', 'AntiBlur'] as $sony_tag) {
	printf('%s was %sfound' . PHP_EOL, $sony_tag, (!isset($data[$sony_tag]) ? 'NOT ' : ''));
}
?>
--EXPECT--
SonyModelID was found
Panorama was found
AntiBlur was found
