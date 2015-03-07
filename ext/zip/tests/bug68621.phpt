--TEST--
Bug #68621	Filename appends the base url.
The ZipArchive::open treats the filename not as defined per php manual on valid file wrappers. It appends the filename to the base url. Hence, creating/manipulating a zip file in memory space would be impossible.
--SKIPIF--
<?php
/* $Id$ */
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php

$zip = new \ZipArchive();
if (!($err = $zip->open("php://memory", \ZipArchive::CREATE)))
{
	die(var_dump($err));
}
$zip->addFromString("test.txt", "sample text");
die(var_dump($zip));

?>
--EXPECTF--
object(ZipArchive)[206]
  public 'status' => int 0
  public 'statusSys' => int 0
  public 'numFiles' => int 1
  public 'filename' => string 'php://memory' (length=12)
  public 'comment' => string '' (length=0)
