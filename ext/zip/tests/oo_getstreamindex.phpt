--TEST--
ZipArchive::getStreamIndex / ZipArchive::getName
--EXTENSIONS--
zip
--FILE--
<?php

$name = __DIR__ . '/getstream.zip';

$zip = new ZipArchive;
$r = $zip->open($name, ZIPARCHIVE::CREATE);
$zip->addFromString('foo.txt', 'foo');
$zip->addFromString('bar.txt', 'bar');
$zip->close();

$r = $zip->open($name);

$zip->addFromString('bar.txt', 'baz', ZipArchive::FL_OVERWRITE);


echo "== Name\n";
$fp = $zip->getStreamName('foo.txt');
var_dump($zip->status);
var_dump(stream_get_contents($fp));
$zip->clearError();
fclose($fp);

echo "== Index\n";
$fp = $zip->getStreamIndex(0);
var_dump($zip->status);
var_dump(stream_get_contents($fp));
$zip->clearError();
fclose($fp);

echo "== Index, changed\n";
$fp = $zip->getStreamIndex(1);
var_dump($zip->status);
$zip->clearError();

echo "== Index, unchanged\n";
$fp = $zip->getStreamIndex(1, ZipArchive::FL_UNCHANGED);
var_dump($zip->status);
var_dump(stream_get_contents($fp));
$zip->clearError();
fclose($fp);

$zip->close();
?>
== Done
--CLEAN--
<?php
$name = __DIR__ . '/getstream.zip';
@unlink($name);
?>
--EXPECTF--
== Name
int(0)
string(3) "foo"
== Index
int(0)
string(3) "foo"
== Index, changed
int(15)
== Index, unchanged
int(0)
string(3) "bar"
== Done
