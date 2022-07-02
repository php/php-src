--TEST--
Phar::buildFromDirectory(), directory exists
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
phar.readonly=0
open_basedir=
--FILE--
<?php

mkdir(__DIR__.'/testdir4');
foreach(range(1, 4) as $i) {
    file_put_contents(__DIR__."/testdir4/file$i.txt", "some content for file $i");
}

try {
    $phar = new Phar(__DIR__ . '/buildfromdirectory4.phar');
    $a = $phar->buildFromDirectory(__DIR__ . '/testdir4');
    asort($a);
    var_dump($a);
} catch (Exception $e) {
    var_dump(get_class($e));
    echo $e->getMessage() . "\n";
}

var_dump(file_exists(__DIR__ . '/buildfromdirectory4.phar'));

?>
--CLEAN--
<?php
unlink(__DIR__ . '/buildfromdirectory4.phar');
foreach(range(1, 4) as $i) {
    unlink(__DIR__ . "/testdir4/file$i.txt");
}
rmdir(__DIR__ . '/testdir4');
?>
--EXPECTF--
array(4) {
  ["file1.txt"]=>
  string(%d) "%stestdir4%cfile1.txt"
  ["file2.txt"]=>
  string(%d) "%stestdir4%cfile2.txt"
  ["file3.txt"]=>
  string(%d) "%stestdir4%cfile3.txt"
  ["file4.txt"]=>
  string(%d) "%stestdir4%cfile4.txt"
}
bool(true)
