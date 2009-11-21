--TEST--
Phar::buildFromDirectory() with non-matching regex
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php

mkdir(dirname(__FILE__).'/testdir', 0777);
foreach(range(1, 4) as $i) {
    file_put_contents(dirname(__FILE__)."/testdir/file$i.txt", "some content for file $i");
}

try {
	$phar = new Phar(dirname(__FILE__) . '/buildfromdirectory.phar');
	var_dump($phar->buildFromDirectory(dirname(__FILE__) . '/testdir', '/\.php$/'));
} catch (Exception $e) {
	var_dump(get_class($e));
	echo $e->getMessage() . "\n";
}

var_dump(file_exists(dirname(__FILE__) . '/buildfromdirectory.phar'));

?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/buildfromdirectory.phar');
foreach(range(1, 4) as $i) {
    unlink(dirname(__FILE__) . "/testdir/file$i.txt");
}
rmdir(dirname(__FILE__) . '/testdir');
?>
--EXPECT--
array(0) {
}
bool(false)
===DONE===
