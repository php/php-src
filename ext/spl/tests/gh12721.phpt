--TEST--
GH-12721 (SplFileInfo::getFilename() segfault in combination with GlobIterator and no directory separator)
--FILE--
<?php

file_put_contents('file1.gh12721', 'hello');

echo "--- No slash ---\n";

foreach (new GlobIterator('*.gh12721') as $fileInfo) {
	echo $fileInfo->getFilename(), "\n";
	echo $fileInfo->getExtension(), "\n";
	echo $fileInfo->getBasename(), "\n";
	var_dump($fileInfo->getFileInfo());
}

echo "--- With slash ---\n";

foreach (new GlobIterator('./*.gh12721') as $fileInfo) {
	echo $fileInfo->getFilename(), "\n";
	echo $fileInfo->getExtension(), "\n";
	echo $fileInfo->getBasename(), "\n";
	var_dump($fileInfo->getFileInfo());
}

?>
--CLEAN--
<?php
@unlink('file1.gh12721');
?>
--EXPECTF--
--- No slash ---
file1.gh12721
gh12721
file1.gh12721
object(SplFileInfo)#4 (2) {
  ["pathName":"SplFileInfo":private]=>
  string(13) "file1.gh12721"
  ["fileName":"SplFileInfo":private]=>
  string(13) "file1.gh12721"
}
--- With slash ---
file1.gh12721
gh12721
file1.gh12721
object(SplFileInfo)#3 (2) {
  ["pathName":"SplFileInfo":private]=>
  string(15) "%sfile1.gh12721"
  ["fileName":"SplFileInfo":private]=>
  string(13) "file1.gh12721"
}
