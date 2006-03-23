--TEST--
Bug #36258 (SplFileObject::getPath() may lead to segfault)
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php

$diriter = new RecursiveIteratorIterator( new RecursiveDirectoryIterator('.') );

foreach ($diriter as $key => $file) {
	var_dump($file->getFilename());
	var_dump($file->getPath());
	break;
}

echo "Done\n";
?>
--EXPECTF--	
string(%d) "%s"
string(%d) "%s"
Done
