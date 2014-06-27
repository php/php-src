--TEST--
Phar::buildFromDirectory() with matching regex
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php

mkdir(dirname(__FILE__).'/testdir5');
foreach(range(1, 4) as $i) {
    file_put_contents(dirname(__FILE__)."/testdir5/file$i.txt", "some content for file $i");
}

try {
	$phar = new Phar(dirname(__FILE__) . '/buildfromdirectory5.phar');
	$a = $phar->buildFromDirectory(dirname(__FILE__) . '/testdir5', '/\.txt/');
	asort($a);
	var_dump($a);
} catch (Exception $e) {
	var_dump(get_class($e));
	echo $e->getMessage() . "\n";
}

var_dump(file_exists(dirname(__FILE__) . '/buildfromdirectory5.phar'));

?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/buildfromdirectory5.phar');
foreach(range(1, 4) as $i) {
    unlink(dirname(__FILE__) . "/testdir5/file$i.txt");
}
rmdir(dirname(__FILE__) . '/testdir5');
?>
--EXPECTF--
array(4) {
  ["file1.txt"]=>
  string(%d) "%stestdir5%cfile1.txt"
  ["file2.txt"]=>
  string(%d) "%stestdir5%cfile2.txt"
  ["file3.txt"]=>
  string(%d) "%stestdir5%cfile3.txt"
  ["file4.txt"]=>
  string(%d) "%stestdir5%cfile4.txt"
}
bool(true)
===DONE===
