--TEST--
Bug #36287 (Segfault with SplFileInfo conversion)
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php

$it = new RecursiveIteratorIterator(new RecursiveDirectoryIterator("."), true);

$idx = 0;
foreach($it as $file)
{
	echo "First\n";
	if("." != $file && ".." != $file)
	{
		var_Dump($file->getFilename());
	}
	echo "Second\n";
	if($file != "." && $file != "..")
	{
		var_dump($file->getFilename());
	}
	if (++$idx > 1)
	{
		break;
	}
}

?>
===DONE===
--EXPECTF--
First
string(%d) "%s"
Second
string(%d) "%s"
First
string(%d) "%s"
Second
string(%d) "%s"
===DONE===
