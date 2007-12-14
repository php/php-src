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
--EXPECTF--
array(33) {
  ["001.phpt"]=>
  string(51) "%s001.phpt"
  ["002.phpt"]=>
  string(51) "%s002.phpt"
  ["003.phpt"]=>
  string(51) "%s003.phpt"
  ["004.phpt"]=>
  string(51) "%s004.phpt"
  ["005.phpt"]=>
  string(51) "%s005.phpt"
  ["006.phpt"]=>
  string(51) "%s006.phpt"
  ["007.phpt"]=>
  string(51) "%s007.phpt"
  ["008.phpt"]=>
  string(51) "%s008.phpt"
  ["009.phpt"]=>
  string(51) "%s009.phpt"
  ["010.phpt"]=>
  string(51) "%s010.phpt"
  ["011.phpt"]=>
  string(51) "%s011.phpt"
  ["012.phpt"]=>
  string(51) "%s012.phpt"
  ["013.phpt"]=>
  string(51) "%s013.phpt"
  ["014.phpt"]=>
  string(51) "%s014.phpt"
  ["015.phpt"]=>
  string(51) "%s015.phpt"
  ["016.phpt"]=>
  string(51) "%s016.phpt"
  ["017.phpt"]=>
  string(51) "%s017.phpt"
  ["018.phpt"]=>
  string(51) "%s018.phpt"
  ["019.phpt"]=>
  string(51) "%s019.phpt"
  ["020.phpt"]=>
  string(51) "%s020.phpt"
  ["021.phpt"]=>
  string(51) "%s021.phpt"
  ["022.phpt"]=>
  string(51) "%s022.phpt"
  ["023.phpt"]=>
  string(51) "%s023.phpt"
  ["024.phpt"]=>
  string(51) "%s024.phpt"
  ["025.phpt"]=>
  string(51) "%s025.phpt"
  ["026.phpt"]=>
  string(51) "%s026.phpt"
  ["027.phpt"]=>
  string(51) "%s027.phpt"
  ["028.phpt"]=>
  string(51) "%s028.phpt"
  ["029.phpt"]=>
  string(51) "%s029.phpt"
  ["030.phpt"]=>
  string(51) "%s030.phpt"
  ["031.phpt"]=>
  string(51) "%s031.phpt"
  ["032.phpt"]=>
  string(51) "%s032.phpt"
  ["033.phpt"]=>
  string(51) "%s033.phpt"
}
===DONE===
