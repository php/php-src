--TEST--
Bug #36258 (SplFileObject::getPath() may lead to segfault)
--FILE--
<?php

$diriter = new RecursiveIteratorIterator( new RecursiveDirectoryIterator('.') );

foreach ($diriter as $key => $file) {
	var_dump($file->getFilename());
	var_dump($file->getPath());
	break;
}

?>
===DONE===
--EXPECTF--
string(%d) "%s"
string(%d) "%s"
===DONE===
