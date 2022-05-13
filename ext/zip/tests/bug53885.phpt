--TEST--
Bug #53885 (ZipArchive segfault with FL_UNCHANGED on empty archive)
--EXTENSIONS--
zip
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
@unlink($fname);
?>
--EXPECTF--
Deprecated: ZipArchive::open(): Using empty file as ZipArchive is deprecated in %s
==DONE==
