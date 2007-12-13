--TEST--
Phar::buildFromIterator() iterator, SplFileInfo as current
--SKIPIF--
<?php if (!extension_loaded("phar")) print "skip"; ?>
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
try {
	chdir(dirname(__FILE__));
	$phar = new Phar(dirname(__FILE__) . '/buildfromiterator.phar');
	var_dump($phar->buildFromIterator(new RegexIterator(new DirectoryIterator('.'), '/^\d{0,3}\.phpt\\z|^\.\\z|^\.\.\\z/'), dirname(__FILE__) . DIRECTORY_SEPARATOR));
} catch (Exception $e) {
	var_dump(get_class($e));
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/buildfromiterator.phar');
__HALT_COMPILER();
?>
--EXPECT--
array(33) {
  ["001.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/001.phpt"
  ["002.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/002.phpt"
  ["003.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/003.phpt"
  ["004.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/004.phpt"
  ["005.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/005.phpt"
  ["006.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/006.phpt"
  ["007.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/007.phpt"
  ["008.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/008.phpt"
  ["009.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/009.phpt"
  ["010.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/010.phpt"
  ["011.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/011.phpt"
  ["012.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/012.phpt"
  ["013.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/013.phpt"
  ["014.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/014.phpt"
  ["015.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/015.phpt"
  ["016.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/016.phpt"
  ["017.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/017.phpt"
  ["018.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/018.phpt"
  ["019.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/019.phpt"
  ["020.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/020.phpt"
  ["021.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/021.phpt"
  ["022.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/022.phpt"
  ["023.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/023.phpt"
  ["024.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/024.phpt"
  ["025.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/025.phpt"
  ["026.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/026.phpt"
  ["027.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/027.phpt"
  ["028.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/028.phpt"
  ["029.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/029.phpt"
  ["030.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/030.phpt"
  ["031.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/031.phpt"
  ["032.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/032.phpt"
  ["033.phpt"]=>
  string(51) "/home/cellog/workspace/php5/ext/phar/tests/033.phpt"
}
===DONE===
