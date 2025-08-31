--TEST--
setMtime
--EXTENSIONS--
zip
--SKIPIF--
<?php
/* $Id$ */
if (!method_exists('ZipArchive', 'setMtimeName')) die('skip libzip too old');
?>
--INI--
date.timezone=UTC
--FILE--
<?php
$dirname = dirname(__FILE__) . '/';
include $dirname . 'utils.inc';
$file = $dirname . '__tmp_oo_set_mtime.zip';

@unlink($file);

$zip = new ZipArchive;
if (!$zip->open($file, ZIPARCHIVE::CREATE)) {
    exit('failed');
}

$zip->addFromString('foo', 'entry #1');
$zip->addFromString('bar', 'entry #2');

$t1 = mktime(0,0,0,12,25,2019);
$t2 = mktime(0,0,0,14,7,2018);

echo "Set 1\n";
$s = $zip->statName('foo');
var_dump($s['mtime'] > $t1);
var_dump($zip->setMtimeName('foo', $t1));
$s = $zip->statName('foo');
// ONLY with 1.6.0 - var_dump($s['mtime'] ==  $t1);

echo "Set 2\n";
$s = $zip->statIndex(1);
var_dump($s['mtime'] > $t2);
var_dump($zip->setMtimeIndex(1, $t2));
$s = $zip->statIndex(1);
// ONLY with 1.6.0 - var_dump($s['mtime'] ==  $t2);

if (!$zip->status == ZIPARCHIVE::ER_OK) {
    echo "failed to write zip\n";
}
$zip->close();

if (!$zip->open($file)) {
    @unlink($file);
    exit('failed');
}

echo "Get 1\n";
$s = $zip->statIndex(0);
var_dump($s['mtime'] ==  $t1);

echo "Get 2\n";
$s = $zip->statName('bar');
var_dump($s['mtime'] ==  $t2);

$zip->close();
@unlink($file);

?>
--EXPECT--
Set 1
bool(true)
bool(true)
Set 2
bool(true)
bool(true)
Get 1
bool(true)
Get 2
bool(true)
