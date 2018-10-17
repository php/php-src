--TEST--
Bug #64739 (Invalid Title and Author data returned)
--SKIPIF--
<?php
extension_loaded("exif") or die("skip need exif");
if (!extension_loaded('mbstring')) die('skip mbstring extension not available');
?>
--FILE--
<?php
echo "Test\n";

$headers1 = exif_read_data(__DIR__ . '/bug64739.jpg');

if ($headers1 === false) {
	echo 'Error, failed to read exif data';
	exit;
}

var_dump($headers1['Title']{0} === '?');
var_dump($headers1['Author']{0} === '?');

ini_set('exif.decode_unicode_motorola', 'UCS-2LE');

$headers2 = exif_read_data(__DIR__ . '/bug64739.jpg');

if ($headers2 === false) {
	echo 'Error, failed to read exif data';
	exit;
}

var_dump($headers2['Title']);
var_dump($headers2['Author']);

?>
Done
--EXPECT--
Test
bool(true)
bool(true)
string(8) "55845364"
string(13) "100420.000000"
Done
