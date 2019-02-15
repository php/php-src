--TEST--
Bug #49072 (feof never returns true for damaged file in zip)
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php
$f = dirname(__FILE__)  . '/bug49072.zip';
$o = new ZipArchive();
if (! $o->open($f, ZipArchive::CHECKCONS)) {
	exit ('error can\'t open');
}
$r = $o->getStream('file1'); // this file has a wrong crc
if (!$r)die('failed to open a stream for file1');
$s = '';
while (! feof($r)) {
	$s .= fread($r,1024);
}
?>
--EXPECTF--
Warning: fread(): Zip stream error: CRC error in %s on line %d
