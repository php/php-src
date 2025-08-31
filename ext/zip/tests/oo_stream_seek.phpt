--TEST--
getStream and seek
--EXTENSIONS--
zip
--SKIPIF--
<?php
if(version_compare(ZipArchive::LIBZIP_VERSION, '1.9.1', '<')) die('skip libzip < 1.9.1');
?>
--FILE--
<?php
var_dump(ZipArchive::LIBZIP_VERSION);
$file = __DIR__ . '/test.zip';
$zip = new ZipArchive;
if (!$zip->open($file)) {
	exit('failed');
}
echo "+ ZipArchive::getStream\n";
$fp = $zip->getStream('bar');
if(!$fp) exit("\n");
var_dump($fp);

var_dump(fseek($fp, 1, SEEK_SET));
var_dump(fread($fp, 2));
var_dump(ftell($fp));
var_dump(fseek($fp, 0, SEEK_SET));
var_dump(fread($fp, 2));
var_dump(ftell($fp));

fclose($fp);

echo "+ ZipArchive::getStream no supported\n";
$fp = $zip->getStream('entry1.txt');
if(!$fp) exit("\n");
var_dump($fp);

var_dump(fseek($fp, 2, SEEK_SET));
var_dump(fread($fp, 2));
fclose($fp);

$zip->close();


echo "+ Zip Stream\n";
$fp = fopen('zip://' . __DIR__ . '/test.zip#bar', 'rb');
if(!$fp) exit("\n");
var_dump($fp);
var_dump(fseek($fp, 1, SEEK_SET));
var_dump(fread($fp, 2));
var_dump(ftell($fp));
var_dump(fseek($fp, 0, SEEK_SET));
var_dump(fread($fp, 2));
var_dump(ftell($fp));
fclose($fp);

?>
--EXPECTF--
string(%d) "%s"
+ ZipArchive::getStream
resource(%d) of type (stream)
int(0)
string(2) "ar"
int(3)
int(0)
string(2) "ba"
int(2)
+ ZipArchive::getStream no supported
resource(%d) of type (stream)

Warning: fseek(): %s does not support seeking in %s
int(-1)
string(2) "en"
+ Zip Stream
resource(%d) of type (stream)
int(0)
string(2) "ar"
int(3)
int(0)
string(2) "ba"
int(2)
