--TEST--
Phar::buildFromIterator() RegexIterator(DirectoryIterator), SplFileInfo as current
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
try {
	chdir(dirname(__FILE__));
	$phar = new Phar(dirname(__FILE__) . '/buildfromiterator8.phar');
	$a = $phar->buildFromIterator(new RegexIterator(new DirectoryIterator('.'), '/^\d{0,3}\.phpt\\z|^\.\\z|^\.\.\\z/'), dirname(__FILE__) . DIRECTORY_SEPARATOR);
	asort($a);
	var_dump($a);
} catch (Exception $e) {
	var_dump(get_class($e));
	echo $e->getMessage() . "\n";
}
?>
===DONE===
--CLEAN--
<?php 
unlink(dirname(__FILE__) . '/buildfromiterator8.phar');
__HALT_COMPILER();
?>
--EXPECTF--
array(33) {
  ["001.phpt"]=>
  string(%d) "%s001.phpt"
  ["002.phpt"]=>
  string(%d) "%s002.phpt"
  ["003.phpt"]=>
  string(%d) "%s003.phpt"
  ["004.phpt"]=>
  string(%d) "%s004.phpt"
  ["005.phpt"]=>
  string(%d) "%s005.phpt"
  ["006.phpt"]=>
  string(%d) "%s006.phpt"
  ["007.phpt"]=>
  string(%d) "%s007.phpt"
  ["008.phpt"]=>
  string(%d) "%s008.phpt"
  ["009.phpt"]=>
  string(%d) "%s009.phpt"
  ["010.phpt"]=>
  string(%d) "%s010.phpt"
  ["011.phpt"]=>
  string(%d) "%s011.phpt"
  ["012.phpt"]=>
  string(%d) "%s012.phpt"
  ["013.phpt"]=>
  string(%d) "%s013.phpt"
  ["014.phpt"]=>
  string(%d) "%s014.phpt"
  ["015.phpt"]=>
  string(%d) "%s015.phpt"
  ["016.phpt"]=>
  string(%d) "%s016.phpt"
  ["017.phpt"]=>
  string(%d) "%s017.phpt"
  ["018.phpt"]=>
  string(%d) "%s018.phpt"
  ["019.phpt"]=>
  string(%d) "%s019.phpt"
  ["020.phpt"]=>
  string(%d) "%s020.phpt"
  ["021.phpt"]=>
  string(%d) "%s021.phpt"
  ["022.phpt"]=>
  string(%d) "%s022.phpt"
  ["023.phpt"]=>
  string(%d) "%s023.phpt"
  ["024.phpt"]=>
  string(%d) "%s024.phpt"
  ["025.phpt"]=>
  string(%d) "%s025.phpt"
  ["026.phpt"]=>
  string(%d) "%s026.phpt"
  ["027.phpt"]=>
  string(%d) "%s027.phpt"
  ["028.phpt"]=>
  string(%d) "%s028.phpt"
  ["029.phpt"]=>
  string(%d) "%s029.phpt"
  ["030.phpt"]=>
  string(%d) "%s030.phpt"
  ["031.phpt"]=>
  string(%d) "%s031.phpt"
  ["032.phpt"]=>
  string(%d) "%s032.phpt"
  ["033.phpt"]=>
  string(%d) "%s033.phpt"
}
===DONE===
