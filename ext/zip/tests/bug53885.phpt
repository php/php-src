--TEST--
Bug #53885 (ZipArchive segfault with FL_UNCHANGED on empty archive)
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
if(version_compare(ZipArchive::LIBZIP_VERSION, '1.6', '>=')) die('skip libzip too recent');
?>
--FILE--
<?php
$fname = __DIR__."/test53885.zip";
if(file_exists($fname)) unlink($fname);
touch($fname);
$nx=new ZipArchive();
$nx->open($fname);
$nx->locateName("a",ZIPARCHIVE::FL_UNCHANGED);
$nx->statName("a",ZIPARCHIVE::FL_UNCHANGED);
?>
==DONE==
--CLEAN--
<?php
$fname = __DIR__."/test53885.zip";
unlink($fname);
?>
--EXPECT--
==DONE==
