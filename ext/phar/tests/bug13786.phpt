--TEST--
Phar: bug #13786: "PHP crashes on phar recreate after unlink"
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php

for ($i = 0; $i < 2; $i++) {
	$fname = "DataArchive.phar";
	$path = dirname(__FILE__) . DIRECTORY_SEPARATOR . $fname;
	$phar = new Phar($path);
	$phar->addFromString($i, "file $i in $fname");
	var_dump(file_get_contents($phar[$i]));
	unset($phar);
	unlink($path);
}

echo("\nWritten files: $i\n");

?>
===DONE===
--EXPECT--
string(26) "file 0 in DataArchive.phar"
string(26) "file 1 in DataArchive.phar"

Written files: 2
===DONE===
